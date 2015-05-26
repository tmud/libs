1. run unzip.bat to unpack FreeImage Base lib and apply patch.
(patch turn offs some plugins - not popular image formats).
2. build Debug/Release version of FreeImage (static lib) by VS2013.
3. build fimage lib project
4. testapp can used for testing
   
Reason:
1. more simple api of lib
2. result executable smaller (support only bmp,ico,png,jpg,gif formats).
3. added render methods for windows HDC.
