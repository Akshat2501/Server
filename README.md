# Server
Http Server to serve html, css and javascript files
The TMWebProjector is a server to serve html,css and javascript files.

In order to use the server, the user first needs to create an object of the 
TMWebProjector class.While creating the object, the user must pass the port number
to which the server should be bound.

The TMWebProjector has 2 methods-
1)start()-It puts the server into the listening/waiting mode in order to accept the
	request from the browser.The home page served will be 'index.html' file.
NOTE - The project folder must contain the 'index.html' file.

2)onRequest(string url,void (*)(Request &,Response &)):
	It accepts 2 arguments-
		(i)The first argument should be the url.
		(ii)The second argument should be the function which should get
			executed on that specified url.
		The function (passed in the second argument) which the user needs 
		to get executed must be of the signature - (Request &,Response &).

NOTE- Each time user needs to map a function against an url,the onRequest() method
	must be used.

When request arrives for a particular url,then the function mapped to that url will
get executed.If no function is mapped to that particular url,then an error page will
be displayed.


The Request class contains the details regarding the method,uri,etc. of the
request that has arrived from the browser.
Following are the functionalities provided by the Request class:
	1)get(string str) method - This method is used to extract data against 'str'
		in the query string of the request.
	2)forward(string str) method - This method is used to specify the resource
 		(url/file) to which the request has to be forwarded.The string 'str' 
		contains that resource to which the request has to be forwarded.
NOTE - If the request is forwarded to a resource which is not present in the project
folder/mapped to a function, then an error page will be displayed. 

The Response sent by the user to the browser must be of HTML type only.
This can be done in the following ways:
1)By directly sending an HTML file using the 'forward' method of the Request class.
2)By using the 'write' method of the Response class.The argument passed to the
	write method can be of type - const char * or string type.
	
After sending the response to the browser,the user should call the'close()' method 
of the Response class.

For detailed working, refer to the sample example.


