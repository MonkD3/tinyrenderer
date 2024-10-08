# Progress log : Lesson 2

## First attempt

By looking at the requirement of the algorithm. I came up with an idea very closely related to the line algorithm.

The idea is to use a left and right sentinel, describing the left and right boundary of the triangle. Then draw the pixel horizontally in between the two sentinels. Each time we draw a pixel line, we move one pixel up and update the sentinel's positions according to the triangle segment they are following. 

The result is quite good and the performance seems ok. But I believe it can be simplified.

```c 
static int compareY(void const * v1, void const * v2){
    int32_t const * v1i = *((int32_t **)v1);
    int32_t const * v2i = *((int32_t **)v2);
    return v1i[Y_COORD] - v2i[Y_COORD];
}

void triangle2D(int32_t const * v1, int32_t const * v2, int32_t const * v3, TGAImage_t * const img, TGAColor_t const * const c){
    // First we sort the point by their height (y coordinates)
    int32_t const * p[3] = {v1, v2, v3};
    qsort(p, 3, sizeof(v1), compareY);
    int32_t const* low = p[0];
    int32_t const* mid = p[1];
    int32_t const* hi  = p[2];

    // Draw horizontally from lo to mid by taking into account the lines low--mid and low--hi 
    // then from mid to hi by taking into account the lines low--hi and mid--hi
    int32_t x_ml = low[X_COORD]; // Beginning of line
    int32_t x_hl = low[X_COORD]; // End of line
                                 
    int32_t adx_ml  = abs(mid[X_COORD] - low[X_COORD]); 
    int32_t ady_ml  = abs(mid[Y_COORD] - low[Y_COORD]);
    int32_t adx_hl  = abs(hi[X_COORD] - low[X_COORD]); 
    int32_t ady_hl  = abs(hi[Y_COORD] - low[Y_COORD]);

    int32_t xdir_ml = mid[X_COORD] > low[X_COORD] ? 1 : -1;
    int32_t xdir_hl = hi[X_COORD] > low[X_COORD] ? 1 : -1;

    int32_t e_ml = 0; // Error of left line
    int32_t e_hl = 0; // Error of right line
    for (int32_t y = low[Y_COORD]; y < mid[Y_COORD]; y++){
        int32_t max = x_ml > x_hl ? x_ml : x_hl;
        int32_t min = x_ml < x_hl ? x_ml : x_hl;
        for (int32_t xd = min; xd <= max; xd++) TGAImage_set(img, c, xd, y); // Draw the line
        e_ml += adx_ml;
        while (2*e_ml > ady_ml){
            x_ml += xdir_ml;
            e_ml -= ady_ml;
        }
        e_hl += adx_hl;
        while (2*e_hl > ady_hl){
            x_hl += xdir_hl;
            e_hl -= ady_hl;
        }
    }

    int32_t x_mh = mid[X_COORD]; // End of line
    int32_t adx_mh  = abs(mid[X_COORD] - hi[X_COORD]); 
    int32_t ady_mh  = abs(mid[Y_COORD] - hi[Y_COORD]);
    int32_t xdir_mh = hi[X_COORD] > mid[X_COORD] ? 1 : -1;
    int32_t e_mh = 0;
    for (int32_t y = mid[Y_COORD]; y < hi[Y_COORD]; y++){
        int32_t max = x_mh > x_hl ? x_mh : x_hl;
        int32_t min = x_mh < x_hl ? x_mh : x_hl;
        for (int32_t xd = min; xd <= max; xd++) TGAImage_set(img, c, xd, y); // Draw the line
        e_mh += adx_mh;
        while (2*e_mh > ady_mh){
            x_mh += xdir_mh;
            e_mh -= ady_mh;
        }
        e_hl += adx_hl;
        while (2*e_hl > ady_hl){
            x_hl += xdir_hl;
            e_hl -= ady_hl;
        }
    }
}
```

### Possible improvements

1. I used `qsort` here but as there are only 3 items some simple comparisons would do the trick.
2. Notice the `while` loops for the update of the sentinels/error. This is to account for the fact that we do not use the steepness of the line and therefore sometimes we have to move the sentinels multiple pixels. It should be simple to compute the number of pixels to be moved and update them at once.
3. As I do not know which sentinel is left and right, I compute the min and max for the horizontal line drawing. It should be possible to compute which way to draw the line prior to the loop.

### Timings 

For the 3 triangles example provided in the lesson on a 200x200 canvas:

- (10, 70), (50, 160), (70, 80). Area : 2500 pixels 
- (180, 50), (150, 1), (70, 180). Area : 4645 pixels 
- (180, 150), (120, 160), (130, 180). Area : 650 pixels

Which makes a total of 7795 pixels.

The performance is the following (**on my laptop**):
```console 
========================== Benchmarks ========================
Sample size k    = 1000
Max iterations   = 10000        ----> Performed : 4164
Required P-value = 0.0100000000 ----> Obtained  : 0.0099628045
Confidence interval : 0.0000201286 +- 0.0000002005 sec
Standard deviation  : 0.0000024572
==============================================================
```

This makes a pixel throughput of : 387 million pixels per second. Not so bad.

## Second attempt 

Based on the possible improvements listed above :
1. Simple sorting with conditionals
2. The displacement of the left and right boundaries is now computed with integer arithmetic rather than a while loop.
3. The drawing direction is precomputed to be able to avoid the min/max computation

```c
void triangle2D(int32_t const * v1, int32_t const * v2, int32_t const * v3, TGAImage_t * const img, TGAColor_t const * const c){
    // First we sort the point by their height (y coordinates)
    int32_t const * p[3] = {v1, v2, v3};
    if (p[0][1] > p[1][1]){
        int32_t const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }
    if (p[1][1] > p[2][1]){
        int32_t const * tmp = p[1];
        p[1] = p[2];
        p[2] = tmp;
    }
    if (p[0][1] > p[1][1]){
        int32_t const * tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;
    }

    int32_t const* low = p[0];
    int32_t const* mid = p[1];
    int32_t const* hi  = p[2];

    int32_t x_ml = low[X_COORD]; // Beginning of line
    int32_t x_hl = low[X_COORD]; // End of line
                                 
    int32_t const adx_ml  = abs(mid[X_COORD] - low[X_COORD]); 
    int32_t const ady_ml  = abs(mid[Y_COORD] - low[Y_COORD]);
    int32_t const adx_hl  = abs(hi[X_COORD] - low[X_COORD]); 
    int32_t const ady_hl  = abs(hi[Y_COORD] - low[Y_COORD]);

    int32_t const xdir_ml = mid[X_COORD] > low[X_COORD] ? 1 : -1;
    int32_t const xdir_hl = hi[X_COORD] > low[X_COORD] ? 1 : -1;
    // As we draw from bottom to top : the edge with the lowest dx is on the left.
    int32_t draw_dir = (mid[X_COORD] - low[X_COORD] > hi[X_COORD] - low[X_COORD]) ? -1 : 1;

    int32_t e_ml = 0; // Error of left line
    int32_t e_hl = 0; // Error of right line
    for (int32_t y = low[Y_COORD]; y < mid[Y_COORD]; y++){
        for (int32_t xd = x_ml; xd != x_hl; xd += draw_dir) TGAImage_set(img, c, xd, y); // Draw the line
        e_ml += adx_ml;
        x_ml += (e_ml/ady_ml)*xdir_ml;
        e_ml -= (e_ml/ady_ml)*ady_ml;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
    }

    int32_t x_mh = mid[X_COORD]; // End of line
    int32_t const adx_mh  = abs(mid[X_COORD] - hi[X_COORD]); 
    int32_t const ady_mh  = abs(mid[Y_COORD] - hi[Y_COORD]);
    int32_t const xdir_mh = hi[X_COORD] > mid[X_COORD] ? 1 : -1;
    draw_dir = x_mh > x_hl ? -1 : 1;
    int32_t e_mh = 0;
    for (int32_t y = mid[Y_COORD]; y < hi[Y_COORD]; y++){
        for (int32_t xd = x_mh; xd != x_hl; xd += draw_dir) TGAImage_set(img, c, xd, y); // Draw the line
        e_mh += adx_mh;
        x_mh += (e_mh/ady_mh)*xdir_mh;
        e_mh -= (e_mh/ady_mh)*ady_mh;

        e_hl += adx_hl;
        x_hl += (e_hl/ady_hl)*xdir_hl;
        e_hl -= (e_hl/ady_hl)*ady_hl;
    }
}
```

![Result](./assets/004_triangle_rasterizing_2.tga) 

We even observe that the right vertex of the green triangle is a little smoother.

### Timings 

```console
========================== Benchmarks ========================
Sample size k    = 1000        
Max iterations   = 10000        ----> Performed : 4532        
Required P-value = 0.0100000000 ----> Obtained  : 0.0097824117
Confidence interval : 0.0000179363 +- 0.0000001755 sec
Standard deviation  : 0.0000021500
==============================================================
```

This is roughly 433 millions pixels per second, or 200FPS on a 1920x1080 screen : nice.

## Optimization

Looking at the `perf report` I see that the majority of the time (~70%) is spent in `TGAImage_set`.

A clear optimization is thus to :

1. Inline the function call 
2. Get rid of the check (unsafe, but faster). To do this I add the explicit function `TGAImage_set_unchecked`.

The resulting timings are the following :
```console
========================== Benchmarks ========================
Sample size k    = 1000        
Max iterations   = 10000        ----> Performed : 6941        
Required P-value = 0.0100000000 ----> Obtained  : 0.0058936874
Confidence interval : 0.0000076755 +- 0.0000000452 sec
Standard deviation  : 0.0000005543
==============================================================
```

That's a very clear gain. We are now at 1 billion pixels per second. This is *very nice*.

## Small bugfixes 

When drawing the head, I encountered bugs with the rendering of the lower part of some triangles. This was
actually due to a wrong assumption on the drawing direction of the first part. I had :
```c 
int32_t draw_dir = (mid[X_COORD] - low[X_COORD] > hi[X_COORD] - low[X_COORD]) ? -1 : 1;
```

Which is actually not correct because it does not take into account the $y$ coordinate. This is a problem when the $x$ coordinates are the same. I thus pivoted towards a trigonometric approach, by computing `atan2`. I did not find a better approach yet.

## Final result 

With all this and some additional linear algebra, we can render a "shaded" version of the head :

![Result](./assets/005_african_head_filled.tga)
