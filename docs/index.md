# uncensored3D



## WebAssembly Hello world ##

I started playing with WebAssembly (back then just Emscripten) several years ago,when the project was quite unmature. Tried to compile a couple of GLES2 sources,got the windows similar to the one below,and forgot about it. But now, I decided to get back to this topic, first because Web finally brought WebGL 2.0, which is much less restricted than its predecessor, hence finally it is less pain for someone who is used to feature rich GL4.0 experimenting with it. Second, the world, as always, goes in a directions, which you're not always happy about, but then there is also nothing you can do about that. And I mean JavaScript. I never liked it, I still don't, but the reality is - web dev is heavily based on it, so I have to master it, despite what I think about the language. And I will probably be writing some JavaScript interface for this app,and hopefully, it will help me to close some gaps in my knowledge base of modern JavaScript. Finally, I have never wrote a complete rendering engine using C only. As a C++ programmer I know that would be a challenge. And I like challenges.

![Spinning rectangle](https://sasmaster.github.io/uncensored3D/helloworld.png)


In this first example I wrote a simple program,which renders just a quad. In fact much of its basis I just copy pasted from this 'getting started' tutorial.
https://github.com/HarryLovesCode/WebAssembly-WebGL-2

I wanted to make sure I don't miss any emscripten specific method, and that repo provided a complete emscripten setup. What is important in this respect is the  function `emscripten_set_main_loop`. It won't work putting the render loop into something we freuqently do in a native app:

    
   
      while(true)
      {
         //render stuff goes here
      }
   
    
   
   Now, during the compilation of this prog, I encountered a couple of issues,which are worth to mention here. First, it looks like emscripten compiler performs quite aggressive optimization of the functions. I had several `inline` methods, which are being eliminated,and contrary to regular compiler,emcc just shows a warning like  "GCCLINK : warning : unresolved symbol: create_texture". So the code will compile,but once you run it in the browser it crashes. On emscripten repo discussion I learnt that putting linker flag 
   `-s LINKABLE=1 ` prevent link time optimization and dead code elimination. I think there is a better way to solve that issue than disabling linker optimization,and I wll check on this one later. [Update] Nope,the last line I wrote doesn't work for C inlined functions. One has to mark those as static as well. Something like this would work:
   
     static inline void foo()
     {
     
     }
 
 Another issue was `unresolved symbol` warnings for glfw functions. I found that using GLFW3 one has to add the following linker flag:
   
   `-s USE_GLFW=3 `
   
   
   And finally, when I launched the app in the browser,with all my functions intact, the shader compiler complained that the GLSL 300 es version declaration wasn't recognized. I first was sure the problem was due to my browser's lack of WebGL2.0 support, but that wasn't the case. One has to use the following linker flag to turn WebGL2.0 support on emscripten side:
   
   `-s USE_WEBGL2=1`
   
   And finally, to get it built as WebAssebly and not wasm one has to use this flag:
   ` -s WASM=1 -s `  
   
   Now,the app is up and running. I need to check if the timer wrapper which uses windows.h under the hood would get compiled ok, as I want to measure frame time. Then I will proceed to structuring my rendering API. C requires a quite different thinking than C++,where we often just wrap everything into some class and forget about that.First noticable limitation I encounter is lack of ready stuff like STL containers, some proper libs like glm math,which I currently replace with some simple header-only math lib.Math is not a problem,but now I am planning also to add support for GLTF files,to render external 3D models,and all those loaders are in C.That will probably force me to write my own GLTF in C. I have never written any C app on a serious scale, so it takes from me some effort at the moment to put some thought into design of the APi, much more than into actual implementation.
   
   
## Emscripten with VS 2017 ##  

To get it working with ![vs-toolsets] (https://github.com/crosire/vs-toolsets) I upgraded Emscripten SDK to the latest version. Funny thing is also that Visual Studio 2017 installs BuildCustomizations and Platforms folders not in C:/MSBuild.. but into   C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\VC\VCTargets   .That's how it looks like at least on my PC after removing VS2015 and installing VS2017 instead. Another important change I had to make was to set explicitly clang compiler to use ###C11 with GNU extensions (-std=gnu11)### under C++ Language standard option. The project uses C and the compiler is set to compile as C code.But for some reason,without telling Clang to use -std=gnu11 it uses -std=c++03 which triggers tons of compile errors in my C code.
   
