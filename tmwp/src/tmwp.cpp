#include<tmwp>      
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
using namespace std;
using namespace tmwp;  

string Request::get(string name)
{
cout<<"---(Get method got called for name)---"<<endl;
string val;
int i,e;
for(i=0;i<this->dataCount;i++)
{
for(e=0;data[i][e]!='\0' && data[i][e]!='=';e++);
if(data[i][e]!='=') continue;    //back to next cycle of for loop
if(strncmp(data[i],name.c_str(),e)==0) break;
}
if(i==this->dataCount)
{
val="";
}
else
{
val=string(data[i]+(e+1));
}
return val;
}

void Request::forward(string forwardTo)
{
this->forwardTo=forwardTo;
}

Response::Response(int clientSocketDescriptor)
{
this->clientSocketDescriptor=clientSocketDescriptor;
this->isClosed=false;
this->headerCreated=false;
}

void Response::createHeader()
{
string header="HTTP/1.1 200 OK\nContent-Type: text/html\n\n"; 
send(clientSocketDescriptor,header.c_str(),header.length()+1,0);
this->headerCreated=true;
}

void Response::write(const char *str)
{
if(str==NULL) return;
int len=strlen(str);
if(len==0) return;
if(!this->headerCreated) createHeader();
send(clientSocketDescriptor,str,len,0);
}

void Response::write(string str)
{
if(str.length()==0) return;
if(!this->headerCreated) createHeader();
send(clientSocketDescriptor,str.c_str(),str.length(),0);
}

void Response::close()
{
if(this->isClosed) return;
closesocket(this->clientSocketDescriptor);
this->isClosed=true;
}


int extensionEquals(const char *left,const char *right)
{
char a,b;
while(*left && *right)
{
a=*left;
b=*right;
if(a>=65 && a<=90) a+=32;
if(b>=65 && b<=90) b+=32;
if(a!=b) return 0;
left++;
right++;
}
return *left==*right;
}

char * getMimeType(char * resource)
{
char * mimeType=NULL;
int len=strlen(resource);
if(len<4) return mimeType;
int lastIndexOfDot=len-1;
while(lastIndexOfDot>0 && resource[lastIndexOfDot]!='.') lastIndexOfDot--;
if(lastIndexOfDot==0) return mimeType;
if(extensionEquals(resource+lastIndexOfDot+1,"html"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"text/html");
} 
if(extensionEquals(resource+lastIndexOfDot+1,"css"))
{
mimeType=(char *)malloc(sizeof(char)*9);
strcpy(mimeType,"text/css");
} 
if(extensionEquals(resource+lastIndexOfDot+1,"js"))
{
mimeType=(char *)malloc(sizeof(char)*16);
strcpy(mimeType,"text/javascript");
}
if(extensionEquals(resource+lastIndexOfDot+1,"png"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"image/png");
} 
if(extensionEquals(resource+lastIndexOfDot+1,"jpg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
} 
if(extensionEquals(resource+lastIndexOfDot+1,"jpeg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
} 
if(extensionEquals(resource+lastIndexOfDot+1,"ico"))
{
mimeType=(char *)malloc(sizeof(char)*13);
strcpy(mimeType,"image/x-icon");
}  
if(extensionEquals(resource+lastIndexOfDot+1,"json"))
{
mimeType=(char *)malloc(sizeof(char)*17);
strcpy(mimeType,"application/json");
}  
return mimeType;
}

char isClientSideResource(char *resource)
{
int i;
for(i=0;resource[i]!='\0' && resource[i]!='.';i++);
if(resource[i]=='\0') return 'N';
return 'Y';
}

Request * parseRequest(char * bytes)
{
char method[11];
char resource[501];
char mimeType[21];
int i,j;
for(i=0;bytes[i]!=' ';i++) method[i]=bytes[i];
method[i]='\0';
i=i+2;
char **data=NULL;
int dataCount=0;
// if method is of GET type
if(strcmp(method,"GET")==0)
{
for(j=0;bytes[i]!=' ';j++,i++)
{
if(bytes[i]=='?') break;
resource[j]=bytes[i];
}
resource[j]='\0';
if(bytes[i]=='?')
{
i++;
int si=i;
dataCount=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&') dataCount++;
i++;
}
dataCount++;
data=(char **)malloc(sizeof(char*)*dataCount);
int *pc=(int *)malloc(sizeof(int)*dataCount);
i=si;
int j=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&')
{
pc[j]=i;
j++;
}
i++;
}
pc[j]=i;
i=si;
j=0;
int howManyToPick;
while(j<dataCount)
{
howManyToPick=pc[j]-i;
data[j]=(char *)malloc(sizeof(char)*(howManyToPick+1));
strncpy(data[j],bytes+i,howManyToPick);
data[j][howManyToPick]='\0';
i=pc[j]+1;
j++;
}
} 
}// Loop related to if method is of GET type,ends here

printf("Request arrived for %s\n",resource);
Request *request=new Request;
request->dataCount=dataCount;
request->data=data;
request->method=(char *)malloc((sizeof(char)*strlen(method))+1);
strcpy(request->method,method);
if(resource[0]=='\0')
{
request->resource=NULL;
request->isClientSideTechnologyResource='Y';
request->mimeType=NULL;
}
else
{
request->resource=(char *)malloc((sizeof(char)*strlen(resource))+1);
strcpy(request->resource,resource);
request->isClientSideTechnologyResource=isClientSideResource(resource);
request->mimeType=getMimeType(resource);
}
return request;
}

TMWebProjector::TMWebProjector(int portNumber)           
{
this->portNumber=portNumber;
}

TMWebProjector::~TMWebProjector()
{
//
}

void TMWebProjector::onRequest(string url,void (*ptrOnRequest)(Request &,Response &))
{
if(url.length()==0 || ptrOnRequest==NULL) return;
requestMappings.insert(pair<string,void (*)(Request &,Response &)>(url,ptrOnRequest));
}

void TMWebProjector::start()         
{
char g;
char requestBuffer[8192];
char responseBuffer[1024];
int serverSocketDescriptor;
int clientSocketDescriptor;
int i,rc,bytesExtracted,successCode,length;   //rc for read count
struct sockaddr_in serverSocketInformation;
struct sockaddr_in clientSocketInformation;
WSADATA wsaData;
WORD ver;
ver=MAKEWORD(1,1);
WSAStartup(ver,&wsaData);
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("Unable to create socket\n");
return;
}
serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
if(successCode<0)
{
printf("Unable to bind socket to port\n");
WSACleanup();
return;
} 
listen(serverSocketDescriptor,10);
int len=sizeof(clientSocketInformation);
while(1)                               //loop to accept connection
{
printf("The server is ready to accept request\n");
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
printf("Unable to accept client connection\n");
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}
bytesExtracted=recv(clientSocketDescriptor,requestBuffer,8192,0);
if(bytesExtracted<0)
{
//later
}
else if(bytesExtracted==0)
{
//
}
else
{
requestBuffer[bytesExtracted]='\0';
printf("%s",requestBuffer);
Request * request=parseRequest(requestBuffer);
while(1)                        //infinite loop to anable the forwarding feature
{
if(request->isClientSideTechnologyResource=='Y')          //if it is client side resource
{
if(request->resource==NULL)
{
FILE *f;
f=fopen("index.html","rb");
if(f!=NULL) printf("Sending index.html\n");
if(f==NULL)
{
f=fopen("index.htm","rb");
if(f!=NULL) printf("Sending index.htm\n");
}
if(f==NULL)
{
char tmp[501];
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TMWeb Projector</title></head><body><h2 style='color:red'>index.html not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\nConnection: close\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 page\n");
closesocket(clientSocketDescriptor);
break;
}
else
{
fseek(f,0,2);    //move the internal pointer to the end of the file
length=ftell(f);
fseek(f,0,0);       //move the internal pointer to the start of the file
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\nConnection: close\n\n",length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;                                               
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}
fclose(f);                                         
closesocket(clientSocketDescriptor);
break;
}  	
}	          
else               //  resource name is present    
{
FILE *f;
f=fopen(request->resource,"rb");
if(f!=NULL) printf("Sending %s\n",request->resource);
if(f==NULL)
{
char tmp[501];
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TMWeb Projector</title></head><body><h2 style='color:red'>Resource /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\nConnection: close\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("Sending 404 error page\n");
break;
}
else
{
fseek(f,0,2);
length=ftell(f);
fseek(f,0,0); 
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\nConnection: close\n\n",request->mimeType,length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;                                            
while(i<length)
{
rc=length-i;
if(rc>1024) rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}                           
fclose(f);                                      
closesocket(clientSocketDescriptor);
break;
}
}	
} 		//isclientsidetechnology
else   
{
//server side resource 

//printf("Server side resource\n");

//printf("%s\n",request->resource);
map<string,void (*)(Request &,Response &)>::iterator iter;
iter=requestMappings.find(string("/")+string(request->resource));
if(iter==requestMappings.end())       // That particular url is not mapped with any function
{
printf("Sending 404 error page\n");
char tmp[501];
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TMWeb Projector</title></head><body><h2 style='color:red'>Resource /%s not found</h2></body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %d\nConnection: close\n\n",strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
break;
}
else
{
Response response(clientSocketDescriptor);
iter->second(*request,response);
if(request->forwardTo.length()>0)    //change the resource part against request and reprocess the request
{
free(request->resource);
request->resource=(char *)malloc((sizeof(char)*request->forwardTo.length())+1);
strcpy(request->resource,request->forwardTo.c_str());
request->isClientSideTechnologyResource=isClientSideResource(request->resource);
request->mimeType=getMimeType(request->resource);
request->forwardTo="";
continue;
}
if(request->data!=NULL)
{
for(int k=0;k<request->dataCount;k++) free(request->data[k]);
free(request->data);
}
break;
}
}
}       // the infinite loop introduced because of forwarding feature ends here
} 	//bytes extracted
}       // the infinite loop related to accept method ends here
closesocket(serverSocketDescriptor);
WSACleanup();
return;
}



