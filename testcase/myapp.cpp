#include<tmwp>
#include<iostream>
using namespace tmwp;
using namespace std;

void function2(Request &request,Response &response)
{
response.write("<!DOCTYPE HTML>");                //The response should be sent in html format.This is done using the 'write' method of the Response class.
response.write("<!HTML lang='en'>");
response.write("<!head>");
response.write("<TITLE>Whatever</TITLE>");
response.write("<meta charset='utf-8'>");
response.write("</head>");
response.write("<body>");
response.write("<h2 style='color:blue'>Thank you for your response.</h2>");
response.write("<BR>");
response.write("You belong to Nanakheda Area and your query has been registerd.");
response.write("<BR>");
response.write("We will get back to you as soon as possible.");
response.write("</body>");
response.write("</html>");
response.close();
}

void function3(Request &request,Response &response)
{
response.write("<!DOCTYPE HTML>");
response.write("<!HTML lang='en'>");
response.write("<!head>");
response.write("<TITLE>Whatever</TITLE>");
response.write("<meta charset='utf-8'>");
response.write("</head>");
response.write("<body>");
response.write("<h2 style='color:blue'>Thank you for your response.</h2>");
response.write("<BR>");
response.write("You belong to Mahananda Nagar Area and your query has been registerd.");
response.write("<BR>");
response.write("We will get back to you as soon as possible.");
response.write("</body>");
response.write("</html>");
response.close();
}

void function1(Request &request,Response &response)
{
string name=request.get("nm");                       //'get' method is used to extract the data that came against 'nm' in the query string.
string area_nmbr=request.get("area_nmbr");           //Similarly 'area_nmbr' and 'msg' are also extracted from the query string.
string query=request.get("msg");
cout<<(name)<<endl;
cout<<(area_nmbr)<<endl;
cout<<(query)<<endl;
if(area_nmbr=="1") request.forward("Nanakheda_Area");            //Based on the Area Number,the request is forwarded to a different resource(url or page).
else if(area_nmbr=="2") request.forward("Mahananda_Nagar_Area");
else request.forward("ErrorPage.html");
}

int main()
{
TMWebProjector server(8080);
server.onRequest("/enquiry",function1);    //The url '/enquiry' is mapped with 'function1'.So whenever the request arrives for '/enquiry' url,'function1' will get executed.
server.onRequest("/Nanakheda_Area",function2);     
server.onRequest("/Mahananda_Nagar_Area",function3);
server.start();
return 0;
}