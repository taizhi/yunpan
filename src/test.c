/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"



/* Used by some code below as an example datatype. */
struct record {const char *precision;double lat,lon;const char *address,*city,*state,*zip,*country; };


void create_objects()
{
		cJSON *root,*thm,*fld;char *out;int i;
		root=cJSON_CreateObject();	
	  cJSON_AddItemToObject(root, "games", thm=cJSON_CreateArray());
	
	for (i=0;i<2;i++)
	{
		cJSON_AddItemToArray(thm,fld=cJSON_CreateObject());	
		cJSON_AddStringToObject(fld,"id","rect");
		cJSON_AddNumberToObject(fld,"kind",2);
		
		cJSON_AddStringToObject(fld,"title_m","rect");
		cJSON_AddStringToObject(fld,"title_s","rect");
		cJSON_AddStringToObject(fld,"descrip","rect");
		cJSON_AddStringToObject(fld,"picurl_m","rect");
		cJSON_AddStringToObject(fld,"url","rect");
		cJSON_AddNumberToObject(fld,"pv",1080);
		cJSON_AddNumberToObject(fld,"hot",1080);
	}
	
	out=cJSON_Print(root);	
	cJSON_Delete(root);	
	printf("%s\n",out);	
	free(out);
}

int main (int argc, const char * argv[]) {

	create_objects();
	
	
	return 0;
}
