# Progress log : lesson 1

## First attempt

This is my first attempt, implemented in [c7a33d5](https://github.com/MonkD3/tinyrenderer/commit/c7a33d5204b503130a88b0be0aa0b02d2351c9a0).

```c
void line(int32_t const x0, int32_t const y0, int32_t const x1, int32_t const y1, TGAImage_t* img, TGAColor_t const * c){
    int32_t const dx = x1 - x0;
    int32_t const dy = y1 - y0;

    // Round up distance
    int32_t const d = (int32_t) ceilf(sqrtf(dx*dx + dy*dy));
    float const fdx = dx * 1.0f/d;
    float const fdy = dy * 1.0f/d;

    for (int32_t i = 0; i < d+1; i++){
        int32_t x = x0 + fdx*i;
        int32_t y = y0 + fdy*i;
        TGAImage_set(img, c, x, y);
    }
}
```
![Image output](./assets/000_lines.tga)

We can observe that for lines close to a 45 degree angle the result is a little bit jagged. In consequences :

1. It's unlikely that the line is symmetric, hence swapping x0/x1 and y0/y1 will not produce the same result. **This contradicts the definition of a mathematical line.**
2. It draws more pixel than what is actually required. **This has a performance impact.**
3. The transition from one side to the other is not smooth. **This is visually unpleasant**

### Timings

The timed example is the following : on a 500x500 canvas, draw one line passing through the center with an angle ranging from 0 to 90 degrees with a step of 10 degrees (i.e. 0, 10, 20,...,90). This produces 10 lines of 500 pixels.

The benchmarks are done through a library of mine : [MonkD3/MicroBenchmarks](https://github.com/MonkD3/MicroBenchmarks)

```c 
    bench_t* b = bench_init(100, 1000, 0.05);
    BENCH_START(b);
    for (int32_t angle = 0; angle <= 90; angle += 10){
        float const rad = angle / 180.0f * M_PI;
        int32_t const x0 = xc - cosf(rad)*WIDTH/2;
        int32_t const x1 = xc + cosf(rad)*WIDTH/2;
        int32_t const y0 = yc - sinf(rad)*HEIGHT/2;
        int32_t const y1 = yc + sinf(rad)*HEIGHT/2;

        line(x0, y0, x1, y1, &img, &red);
    }
    BENCH_STOP(b);
    BENCH_OUTPUT(b);
    bench_destroy(b);
```

The results are the following :
```console
[ monk tinyrenderer main ]$  MODE=bench make
<compilation in benchmark mode>
[ monk tinyrenderer main ]$  ./build/main
========================== Benchmarks ========================
Sample size k    = 100
Max iterations   = 1000         ----> Performed : 102
Required P-value = 0.0500000000 ----> Obtained  : 0.0187361751
Confidence interval : 0.0000436628 +- 0.0000008181 sec
Standard deviation  : 0.0000041229
==============================================================
```

With this, we can expect 98% of the samples to be in the range $[42.8µs, 44.5µs]$

---

## Second attempt 

This second attempt, implemented in [bbd6e6c](https://github.com/MonkD3/tinyrenderer/commit/bbd6e6cef1580f9b717a94e23c01986fa37ad455) aims at eliminating the problems observed in the first attempt. The solutions are described below :

1. We now always define $x_0 < x_1$, in this manner the line will always be symmetric because it will always be drawn in the same order. (and as the algorithm is deterministic it will thus lead to the same result)
2. We now draw the **minimum number of pixel required to have a continuous line**. This is done by using the maximum distance between the two points : $\| p_1 - p0 \|_\inf$ where $p_1 = (x_1, y_1), p_0 = (x_0, y_0)$.
3. We now draw the line as a set of horizontal (or vertical, depending on the slope) lines of pixels, each distant from exactly one pixel. There are clear rules on which pixels to draw that depends on the error between the mathematical line and the center of the pixel.

This will be further explained in a future document.

The resulting algorithm is the following :

```c 
void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, TGAImage_t* img, TGAColor_t const * c){

    // Enforce x0 < x1 
    if (x0 > x1) {
        int32_t tmp = x0;
        x0 = x1;
        x1 = tmp;

        tmp = y0;
        y0 = y1;
        y1 = tmp;
    }

    int32_t const dx = x1 - x0;  // dx is positive by definition
    int32_t const dy = y1 - y0;
    int32_t const ady = abs(dy);
    int32_t const y_direction = dy / ady;
    int32_t e  = 0;

    if (dx > ady){
        for (int32_t i = 0; i < dx; i++){
            TGAImage_set(img, c, x0, y0);
            x0++;
            e += ady;
            if (2*e > dx){
                e -= dx;
                y0 += y_direction;
            }
        }
    } else {
        for (int32_t i = 0; i < ady; i++){
            TGAImage_set(img, c, x0, y0);
            y0 += y_direction;
            e += dx;
            if (2*e > ady){
                e -= ady;
                x0++;
            }
        }
    }
}
```
![Image output](./assets/001_lines.tga)

Note : this algorithm still has a small issue. When $y_0 = y_1$ the computation $y_{direction} = \frac{y_1 - y_0}{|y_1 - y_0|}$ is undefined. We can fix this by simply either by using a condition or its branchless equivalent.

### Timings 

```
========================== Benchmarks ========================
Sample size k    = 1000
Max iterations   = 10000        ----> Performed : 1971
Required P-value = 0.0100000000 ----> Obtained  : 0.0097034992
Confidence interval : 0.0000195420 +- 0.0000001896 sec
Standard deviation  : 0.0000023235
==============================================================
```

## Mesh rendering 

The second attempt yield good result and seems efficient. It is able to draw 10 lines of 500 pixels, hence a total of 5k pixels in approximately 20µs. This is equivalent to 250 millions pixels per second. 

> [!NOTE] 
> The *pixel throughput* should actually be *very slightly* superior to 250M. This is because the benchmark accounts for the sin/cos required for the angle of the lines, which is expensive but should be negligible because we compute 40 trigonometric functions and draw 5k pixels

We can thus use this algorithm to render an object. This is done as follows (commented for clarity):

```c 
const int32_t dv = obj.dv; // Dimension of the vertices
for (uint64_t i = 0; i < obj.nf; i++){  // obj.nf = number of faces (=triangles in this case)
    uint64_t j = obj.fx[i];             // obj.fx = index of the face
    uint64_t nvf = obj.fx[i+1] - j;     // Number of vertex for this face (=3 for triangles)
    for (uint64_t k = 0; k < nvf; k++){
        // Indexing is as follows :
        // obj.v is the list of vertex coordinates : v1x v1y v1z v2x v2y v2z ...
        // obj.fvx is the index of the vertices belonging to the face
        int32_t x0 = (obj.v[obj.fvx[j+k]*dv] + 1.) * WIDTH / 2;
        int32_t y0 = (obj.v[obj.fvx[j+k]*dv+1] + 1.) * HEIGHT / 2;
        int32_t x1 = (obj.v[obj.fvx[j+(1+k)%nvf]*dv] + 1.) * WIDTH / 2;
        int32_t y1 = (obj.v[obj.fvx[j+(1+k)%nvf]*dv + 1] + 1.) * HEIGHT / 2;
        line(x0, y0, x1, y1, &img, &white);
    }
}
```

The indexing is quite hard to comprehend if you're not used to represent multi-dimensional arrays as a 1D array. The idea is that instead of making multiple indexing (e.g. `Matrix[i][j]`) we do a single indexing call with some arithmetic (e.g. `Matrix[i*ncol + j]`). We do this for the vertices.

![Resulting image](./assets/002_african_head_2D_render.tga)
