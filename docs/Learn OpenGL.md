## 05 Hello Triangle

### Object Types

> We manage memory via a so-called vertex buffer objects (VBO) that can store a large number of vertices in the GPU’s memory. The advantage of using those buffer objects is that we can send large batches of data all at once to the graphics card, and keep it there if there’s enough memory left, without having to send data one vertex at a time. Sending data to the graphics card from the CPU is relatively slow, so wherever we can we try to send as much data as possible at once. Once the data is in the graphics card’s memory the vertex shader has almost instant access to the vertices making it extremely fast.

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=29&selection=4,0,10,14|Learn OpenGL - Joey de Vries, page 29]]

> A vertex array object (also known as VAO) can be bound just like a vertex buffer object and any subsequent vertex attribute calls from that point on will be stored inside the VAO. This has the advantage that when configuring vertex attribute pointers you only have to make those calls once and whenever we want to draw the object, we can just bind the corresponding VAO. This makes switching between different vertex data and attribute configurations as easy as binding a different VAO. All the state we just set is stored inside the VAO.

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=35&selection=129,0,134,56|Learn OpenGL - Joey de Vries, page 35]]

> An EBO is a buffer, just like a vertex buffer object, that stores indices that OpenGL uses to decide what vertices to draw. This so called indexed drawing is exactly the solution to our problem.

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=38&selection=106,59,108,55|Learn OpenGL - Joey de Vries, page 38]]

## 06 Shaders

> glBufferData is a function specifically targeted to copy user-defined data into the currently bound buffer. Its first argument is the type of the buffer we want to copy data into: the vertex buffer object currently bound to the GL_ARRAY_BUFFER target. The second argument specifies the size of the data (in bytes) we want to pass to the buffer; a simple sizeof of the vertex data suffices. The third parameter is the actual data we want to send. 
> 
> The fourth parameter specifies how we want the graphics card to manage the given data. This can take 3 forms: 
> 
> • GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times. 
> • GL_STATIC_DRAW: the data is set only once and used many times. 
> • GL_DYNAMIC_DRAW: the data is changed a lot and used many times.

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=29&selection=68,0,99,48|Learn OpenGL - Joey de Vries, page 29]]

> The function glVertexAttribPointer has quite a few parameters so let’s carefully walk through them: 
> 
> • The first parameter specifies which vertex attribute we want to configure. Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0). This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0. 
> • The next argument specifies the size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values. 
> • The third argument specifies the type of the data which is GL_FLOAT (a vec* in GLSL consists of floating point values). 
> • The next argument specifies if we want the data to be normalized. If we’re inputting integer data types (int, byte) and we’ve set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float. This is not relevant for us so we’ll leave this at GL_FALSE. 
> • The fifth argument is known as the stride and tells us the space between consecutive vertex attributes. Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride. Note that since we know that the array is tightly packed (there is no space between the next vertex attribute value) we could’ve also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed). Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute but we’ll get to see more examples of that later on. 
> • The last parameter is of type void* and thus requires that weird cast. This is the offset of where the position data begins in the buffer. Since the position data is at the start of the data array this value is just 0. We will explore this parameter in more detail later on.

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=34&selection=38,0,148,56|Learn OpenGL - Joey de Vries, page 34]]

## 07 Textures

> • GL_NEAREST_MIPMAP_NEAREST: takes the nearest mipmap to match the pixel size and uses nearest neighbor interpolation for texture sampling. 
> • GL_LINEAR_MIPMAP_NEAREST: takes the nearest mipmap level and samples that level using linear interpolation. 
> • GL_NEAREST_MIPMAP_LINEAR: linearly interpolates between the two mipmaps that most closely match the size of a pixel and samples the interpolated level via nearest neighbor interpolation. 
> • GL_LINEAR_MIPMAP_LINEAR: linearly interpolates between the two closest mipmaps and samples the interpolated level via linear interpolation

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=58&selection=32,0,53,59|Learn OpenGL - Joey de Vries, page 58]]

## 08 Transformations

![[Pasted image 20240304183519.png]]

![[Pasted image 20240304183531.png]]

## 09 Coordinates

![[Pasted image 20240304203944.png]]

> OpenGL stores all its depth information in a z-buffer, also known as a depth buffer. GLFW automatically creates such a buffer for you (just like it has a color-buffer that stores the colors of the output image). The depth is stored within each fragment (as the fragment’s z value) and whenever the fragment wants to output its color, OpenGL compares its depth values with the z-buffer. If the current fragment is behind the other fragment it is discarded, otherwise overwritten. This process is called depth testing and is done automatically by OpenGL.

[[Computación/Learn OpenGL - Joey de Vries.pdf#page=92&selection=16,0,27,57|Learn OpenGL - Joey de Vries, page 92]]

## 10 Camera

[1] en.wikipedia.org/wiki/Gram-Schmidt_process

## 13 Basic Lighting

[2] www.lighthouse3d.com/tutorials/glsl-tutorial/the-normal-matrix/

Gouraud shading: Phong shading done in the vertex shader.

## 16 Light Casters

[3] www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation 


IDEAAA: MOVER LUCES ALREDEDOR DE FACE TUTORIAL MAS ADELANTE;;;;

IDEA 2: WIND WAKER LIGHT TRANSITION SLERP

## 20 Models

[4] assimp.sourceforge.net/lib_html/postprocess_8h.html
