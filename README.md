# Simple Ray Tracer
Based on [_Ray Tracing in One Weekend_](https://raytracing.github.io/books/RayTracingInOneWeekend.html). Try to learn raytracing fast.

### 性能对比

目标图片参数：

```C++
int spp=100;
constexpr auto aspect_ratio = 3.0 / 2.0;
constexpr int image_width = 1200;
```

2560$\times$1600  600spp示例图像

![random_balls](README.assets/random_balls.png)

CPU:10710U 35W

| BOX      | 线程 | 时间    |
| -------- | ---- | ------- |
| NO       | 10   | 305.57s |
| Only Box | 10   | 501.3s  |
| BVH      | 10   | 45.77s  |

加速效果明显。关于时间复杂度的分析待完成。