# Comparison between Barycentric and Line based rendering

The goal of this section is to compare the barycentric approach versus the line approach. 
The comparison is done on the `african_head.obj` model, without texture but with basic lighting on a 1000x1000 pixels TGA image.

## Barycentric 

1. Compute a bounding box of the triangle
2. For every pixel of that box: 
   - Compute its barycentric coordinates
   - If one coordinate is negative, go to the next iteration. Else draw the pixel.

The problem with this formulation is that it performs a lot of useless computations. On the other hand, it is embarrassingly parallel (every pixel can be computed independently).

### Timing 

```
========================== Benchmarks ========================
Sample size k    = 100
Max iterations   = 1000         ----> Performed : 101
Required P-value = 0.0100000000 ----> Obtained  : 0.0071037467
Confidence interval : 0.0105001405 +- 0.0000745903 sec
Standard deviation  : 0.0002840011
==============================================================
```

This makes a solid 100 frames per second for a 1000x1000 image.

![Result](./assets/006_Lesson2_further_barycentric_rendering.tga)

## Line based 

Draw the triangle using horizontal lines. To do this :

1. Keep in memory the leftmost $x_l$ and rightmost $x_r$ values.
2. Draw a line between $x_l$ and $x_r$
3. Update $x_l$ and $x_r$ according to the edges of the triangle, just as we did when drawing a line.

This approach has the advantage of only using the pixels inside the triangle. On the other hand, it is inherently sequential (and thus hard to parallelize) and it needs a lot of intermediate computations to get all of the $x, y$ and $z$ values. Indeed, as we are following a 3D line we have to perform Bresenham algorithm on the $x$ and $z$ coordinates (as we iterate over $y$).

```
========================== Benchmarks ========================
Sample size k    = 100
Max iterations   = 1000         ----> Performed : 100
Required P-value = 0.0100000000 ----> Obtained  : 0.0085957235
Confidence interval : 0.0038064219 +- 0.0000327190 sec
Standard deviation  : 0.0001245767
==============================================================
```

This gives a little more then 250 FPS. 

![Result](./assets/007_Lesson2_further_pixel_rendering.tga)


## Final words 

Overall, the line based implementation does not have a lot of attractive features. Sure, it's about 3 times faster in this use case. **BUT:**

- The barycentric approach already yields 100FPS, which is good enough. 
- This is for an unoptimized and un-parallelized version. The Barycentric approach is very suitable to run on a GPU, which would then give *very* different performance results.
- The line based approach is a lot less straightforward to implement and extend. For example, the barycentric approach can easily be extended to take texture into account as we already have the coordinates on the given triangle, we can just reuse it on the texture triangle and be done with it. Implementing textures on the line based approach would required to either follow the lines of the texture triangle (and hence duplicating the work) or interpolate the coordinates, which would besically be equivalent to the barycentric version. 

With these observations, it is quite clear that the barycentric formulation is a lot more attractive for 3D rendering. Although the line based approach definitely has a use case in 2D single-threaded rendering.


