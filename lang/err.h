/*
MIT License

Copyright (c) 2016 Lixing Ding <ding.lixing@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef __LANG_ERR_INC__
#define __LANG_ERR_INC__

#define ERR_SysError            1

#define ERR_NotEnoughMemory     200
#define ERR_NotImplemented      201
#define ERR_StaticNumberOverrun 202
#define ERR_StackOverflow       203
#define ERR_ResourceOutLimit    204

#define ERR_InvalidToken        300
#define ERR_InvalidSyntax       301
#define ERR_InvalidLeftValue    302
#define ERR_InvalidSementic     303

#define ERR_InvalidByteCode     400
#define ERR_InvalidInput        401
#define ERR_InvalidCallor       402
#define ERR_NotDefinedId        403
#define ERR_NotDefinedProp      404
#define ERR_HasNoneElement      405
#define ERR_HasNoneProperty     406

#endif /* __LANG_ERR_INC__ */

