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
