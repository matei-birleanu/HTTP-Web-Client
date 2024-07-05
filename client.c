#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST "34.246.184.49"
#define PORT 8080
#define REGISTER_URL "/api/v1/tema/auth/register"
#define PAYLOAD_TYPE "application/json"
#define LOGIN_URL "/api/v1/tema/auth/login"
#define LOGOUT_URL "/api/v1/tema/auth/logout"
#define ACCESS_URL "/api/v1/tema/library/access"
#define BOOKS_URL "/api/v1/tema/library/books"
char *token = NULL;
char *cookie = NULL;
void register_usr(){
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    char username[1000], password[1000];
    // citesc datele de inregistrare
    printf("username=");
    scanf("%s",username);
    printf("password=");
    scanf("%s",password);
    JSON_Value *user_val = json_value_init_object();
    JSON_Object *user_obj = json_value_get_object(user_val);
    json_object_set_string(user_obj,"username",username);
    json_object_set_string(user_obj,"password",password);
    // creez obiectul de tip json cu campurile necesare
    char *s = json_serialize_to_string(user_val);
    // creez mesajul si deschid conexiunea
    message = compute_post_request(HOST,REGISTER_URL,PAYLOAD_TYPE,&s,1,NULL,0,NULL);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    // trimit mesajul, primesc raspuns inchid conexiunea
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    // in functie de tipul raspunsului de la server informez user ul
    if(strstr(response,"error") != 0){
        char * aux = strstr(response,"taken");
        if(aux != 0)
            printf("Error - Not authorized username already taken\n");
    }
    else{
        printf("200 - OK - User registered successfully!\n");
    }
    // eliberez memoria alocata
    free(message);
    free(response);
    json_free_serialized_string(s);
    json_value_free(user_val);
}

void login_usr(){
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    char username[1000], password[1000];
    // citesc datele de logare
    printf("username=");
    scanf("%s",username);
    printf("password=");
    scanf("%s",password);
    JSON_Value *user_val = json_value_init_object();
    JSON_Object *user_obj = json_value_get_object(user_val);
    json_object_set_string(user_obj,"username",username);
    json_object_set_string(user_obj,"password",password);
    // creez obiectul de tip json cu campurile necesare
    char *s = json_serialize_to_string(user_val);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    // creez mesajul si deschid conexiunea
    message = compute_post_request(HOST,LOGIN_URL,PAYLOAD_TYPE,&s,1,NULL,0,NULL);
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);

    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    if(strstr(response,"error")!=0){
        printf("Login error\n");
    }
    else{
        printf("200 - Successful login\n");
        char *aux = strstr(response, "Set-Cookie");
        if(cookie != NULL){
            free(cookie);
            cookie = NULL;
        }
        // aloc noul cookie de sesiune
        cookie = calloc(127,sizeof(char));
        memcpy(cookie, aux + 12, 126);
        strtok(cookie,";");
        // ma asigur ca tokenul JWT este NULL
        if(token != NULL){
            free(token);
            token = NULL;
        }         
    }
    json_free_serialized_string(s);
    json_value_free(user_val);
    json_value_free(json_response);
    free(message);
    free(response);
}
void enter_lib(){

    if(cookie == NULL){
        printf("You need to login first\n");
        return;
    }
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    //comunic cu severul
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(HOST,ACCESS_URL,NULL,&cookie,1,NULL);
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);

    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    // parsez raspunsul serverului cu ajutorul functiilor specifice JSON
    if(strstr(response,"error") != 0){
        printf("You do not have library access!\n");
    }
    else{
        printf("Succes - Granted access to library\n");
        if(token){
            free(token);
            token = NULL;
        }
        // aloc un nou token JWT si il initalizez cu cel primit de la server
        token = calloc(400, sizeof(char));
        strcpy(token, "Authorization: Bearer ");
        strcat(token, json_object_get_string(json_object(json_response), "token"));
    }
    json_value_free(json_response);
    free(message);
    free(response);
}
void get_books(){
    // verific cerintele necesare pentru a accesa cartile
    if(cookie == NULL){
        printf("You need to login first\n");
        return;
    }
    if(token == NULL){
        printf("You do not have library access!\n");
        return;
    }
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    //comunic cu serverul
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(HOST,BOOKS_URL,NULL,&cookie,1,token);
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    if(strstr(response,"error") != 0){
        char * aux = strstr(response,"error");
        printf("Not authorized\n %s",aux);
    }
    else{
        //caut inceputul secventei JSON din raspuns si o afisez
        char *aux = NULL;
        aux = strstr(response,"{");
        // tratez cazul cand n am nicio carte adaugata
        if(aux == NULL){
            printf("The library is empty\n");
            return;
        }
        aux[strlen(aux)-1] = '\0';
        printf("Succes - Retrieved books succesfully\n");
        printf("%s",aux);
    }
    free(message);
    free(response);
    json_value_free(json_response);
}

void add_book(){
    // verific cerintele minime pentru a adauga o carte
    if(cookie == NULL){
        printf("You need to login first\n");
        return;
    }
    if(token == NULL){
        printf("You do not have library access!\n");
        return;
    }
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    char title[2000], author[2000], genre[2000], publisher[2000] , count[2000];
    int page_count;
    JSON_Value *book_value = json_value_init_object();
    JSON_Object *book_object = json_value_get_object(book_value);
    // citesc noua linie si verific validitatea informatiilor introduse
    getc(stdin);
    printf("title=");
    fgets(title, LINELEN, stdin);
    json_object_set_string(book_object, "title", title);
    printf("author=");
    fgets(author, LINELEN, stdin);
    if(atoi(author) != 0){
        printf("Wrong author format\n");
        return;
    }
    json_object_set_string(book_object, "author", author);
    printf("genre=");
    fgets(genre, LINELEN, stdin);
    json_object_set_string(book_object, "genre", genre);
    printf("publisher=");
    fgets(publisher, LINELEN, stdin);
    if(atoi(publisher) != 0){
        printf("Wrong publisher format\n");
        return;
    }
    json_object_set_string(book_object, "publisher", publisher);
    printf("page_count=");
    fgets(count, LINELEN, stdin);
    if(atoi(count) == 0){
        printf("Invalid format for page_count\n");
        return;
    }
    page_count = atoi(count);
    json_object_set_number(book_object, "page_count", page_count);
    
    char *s = json_serialize_to_string(book_value);
    //comunic cu serverul
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(HOST,BOOKS_URL,PAYLOAD_TYPE,&s,1,&cookie,1,token);

    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    // tratez cazul de eroare/succes
    if(strstr(response,"error")!= 0){
        char *aux = strstr(response,"error");
        printf("Error - %s\n",aux);
    }else
    {
        printf("Succes - Book added succesfully\n");
    }
    free(message);
    free(response);
    json_free_serialized_string(s);
    json_value_free(book_value);
    json_value_free(json_response);
}

void get_book(){
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    if(token == NULL){
        printf("You do not have library access!\n");
        return;
    }
    // citesc linia de dupa get_book
    getc(stdin);
    char id_str[LINELEN];
    char route[LINELEN];
    printf("id=");
    fgets(id_str,LINELEN,stdin);
    if(atoi(id_str) == 0){
        printf("Invalid format for id\n");
        return;
    }
    // construiesc url specific cu id ul cartii
    int id = atoi(id_str);
    sprintf(route, "/api/v1/tema/library/books/%d", id);
    message = compute_get_request(HOST,route,NULL,&cookie,1,token);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    char *json_str_response = basic_extract_json_response(response);
    JSON_Value *json_response = json_parse_string(json_str_response);
    if(strstr(response,"error") != 0){
        printf("ERROR in getting the book\n");
    }else{
        // afisez informatiile primite de la server despre carte
        int page_count;
        char title[LINELEN], author[LINELEN], publisher[LINELEN], genre[LINELEN];
        id = (int)json_object_get_number(json_object(json_response), "id");
        strcpy(title,json_object_get_string(json_object(json_response), "title"));
        strcpy(author,json_object_get_string(json_object(json_response), "author"));
        strcpy(publisher,json_object_get_string(json_object(json_response), "publisher"));
        strcpy(genre,json_object_get_string(json_object(json_response), "genre"));
        page_count = (int)json_object_get_number(json_object(json_response), "page_count");
        printf("Retrieved book succesfully\n");

        printf("id:\t\t%d\n", id);
        printf("title:\t\t%s\n", title);
        printf("author:\t\t%s\n", author);
        printf("publisher:\t%s\n", publisher);
        printf("genre:\t\t%s\n", genre);
        printf("page_count:\t%d\n", page_count);
    }
    free(message);
    free(response);
    json_value_free(json_response);
}
void delete_book(){
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    if (token == NULL) {
        printf("You do not have library access!\n");
        return;
    }
    // citesc newline
    getc(stdin);
    char id_str[LINELEN];
    char route[LINELEN];
    printf("id=");
    fgets(id_str,LINELEN,stdin);
    if(atoi(id_str) == 0){
        printf("Invalid format for id\n");
        return;
    }
    int id = atoi(id_str);
    // construiesc url specific cu id ul cartii
    sprintf(route, "/api/v1/tema/library/books/%d", id);
    // comunic cu serverul
    message = compute_delete_request(HOST,route,PAYLOAD_TYPE,&cookie, 1, token);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);

    if(strstr(response, "error") != 0){
        printf("Error with deleting the book\n");
    }else{
        printf("Deleted book successfully\n");
    }
    free(message);
    free(response);
}

void logout(){
    char *message = NULL;
    char *response = NULL;
    int sockfd;
    if(cookie == NULL){
        printf("You are not logged in!\n");
        return;
    }
    // comunic cu serverul
    message = compute_get_request(HOST,LOGOUT_URL,NULL,&cookie,1,NULL);
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(sockfd,message);
    response = receive_from_server(sockfd);
    close_connection(sockfd);
    if(strstr(response,"error") != 0){
        printf("ERROR logout\n");
    }else{
        printf("Logout succesfull\n");    
    }
    free(cookie);
    cookie = NULL;
    if(token){
        free(token);
        token = NULL;
    }
    free(message);
    free(response);
}
int main(int argc, char *argv[])
{
    char command[1000];
    while(1){
        scanf("%s",command);
        if(!strncmp(command,"register",9)){
            register_usr();
            continue;
        }
        if(!strncmp(command,"login",6)){
            login_usr();
            continue;
        }
        if(!strncmp(command,"enter_library",14)){
            enter_lib();
            continue;
        }
        if(!strncmp(command,"get_books",10)){
            get_books();
            continue;
        }
        if(!strncmp(command,"add_book",9)){
            add_book();
            continue;
        }
        if(!strncmp(command,"get_book",9)){
            get_book();
            continue;
        }
        if(!strncmp(command,"delete_book",12)){
            delete_book();
            continue;
        }
        if(!strncmp(command,"logout",7)){
            logout();
            continue;
        }
        if(!strncmp(command,"exit",4)){
            free(cookie);
            free(token);
            break;
        }
        printf("Unknknwon command\n");
    }

    return 0;
}
