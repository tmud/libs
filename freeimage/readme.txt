1. run unzip.bat to unpack FreeImage Base lib and apply patch.
(patch turn offs some plugins - not popular image formats).
2. build Debug/Release version of FreeImageLib (static lib) by VS2013.
3. build fimage dll project (dll wrapper for FreeImage). 
4. testapp can used for testing
   
Reason:
1. more simple lib
2. result dll smaller (support only bmp,ico,png,jpg,gif formats).
3. added render methods for windows HDC.
