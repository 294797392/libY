
# 基本概念

* 矢量：既有大小又有方向的量叫矢量
* 仿射变换：缩放/旋转变换 + 平移变换

注意：本文基于2D坐标系讨论矩阵


## 学习视频
https://edu.csdn.net/course/detail/24525?utm_medium=distribute.pc_relevant.none-task-course-BlogCommendFromMachineLearnPai2-1.control&dist_request_id=1328602.71184.16152764358377615&depth_1-utm_source=distribute.pc_relevant.none-task-course-BlogCommendFromMachineLearnPai2-1.control


## 矩阵的运算

* 矩阵相加：
* 矩阵减法：
* 矩阵乘法：
* 矩阵除法：
* 转置矩阵：将矩阵的行列互换得到的新矩阵称为转置矩阵 TODO：转置矩阵的运算性质



## 齐次坐标
> 百度百科解释：齐次坐标就是将一个原本是n维的向量用一个n+1维向量来表示。

假设有一个变量：
$$P = （x，y）$$

齐次坐标有两个功能：

1. 区分变量P到底是一个向量还是一个坐标点（因为增加了一个维度，所以增加的这个维度就可以用来表示到底是向量还是坐标点）
P =（x，y，0），该齐次坐标把变量p表示为一个向量
P =（x，y，1），该齐次坐标把变量p表示为一个坐标点

2. 方便图像的变换，把一个2D坐标点转换成一个齐次坐标，这样就可以进行仿射变换



## 图像变换（Transform）

2D图像的变换原理就是对一张图像里的每一个像素点做一次矩阵的运算操作，运算后所得到的像素坐标点就是变换后的点。旋转和缩放是乘法运算，平移是加法运算。下面列举了几个基本的变换矩阵。

### 平移变换

### 旋转变换

设旋转角度为x，那么有如下矩阵：

$$\begin{Bmatrix}
cos(x) & -sin(x) \\ 
sin(x) &  cos(x) \\
\end{Bmatrix}$$

### 缩放变化

设x轴缩放倍数为sx，y轴缩放倍数为sy，那么有如下矩阵：
$$\begin{Bmatrix}
sx &  0 \\ 
0  & sy \\
\end{Bmatrix}$$


### 仿射变换

- 仿射变换包含平移/旋转/缩放这三个功能。
- 仿射变换可以把一个平面的坐标转换到另外一个平面。
- 通过仿射变换,可以将平面中的同一个点从一套坐标转换到另一套坐标。
- 仿射变换 = 旋转/缩放 + 平移
- **旋转/缩放矩阵是一个2x2的矩阵，平移是一个2x1的矩阵**
- 设平移矩阵为A，旋转矩阵为B，缩放矩阵为C，那么仿射矩阵为ABC，注意运算顺序一定要正确，因为矩阵的乘法不满足交换律，顺序不对会导致计算出错误的变换矩阵。

设变换前的坐标是V1，变换后的坐标点是V2，旋转/缩放的矩阵是M1，平移矩阵是M2:

<center>

$ V1 = \begin{vmatrix} x1 \\ y1 \\ \end{vmatrix}, V2 = \begin{vmatrix} x2 \\ y2 \\ \end{vmatrix}, M1 = \begin{pmatrix} M1_{11} & M1_{12} \\ M1_{21} & M1_{22} \\ \end{pmatrix}, M2 = \begin{pmatrix} M2_{11} \\ M2_{21} \\ \end{pmatrix} $

</center>

那么仿射变换的公式就是:
$$ V2 = M1 * V1 + M2 $$

#### 推算：

1. 对点V1先运行旋转/缩放变换：

<center>

$ \begin{pmatrix} M1_{11} & M1_{12} \\ M1_{21} & M1_{22} \\ \end{pmatrix} $ $ \begin{vmatrix} x1 \\ y1 \\ \end{vmatrix} $ = $ \begin{cases} x2=M1_{11}x1+M1_{12}y1 \\ y2=M1_{21}x1+M1_{22}y1 \\ \end{cases} $

</center>


2. 再对点V1运行平移变换：

<center>

$ \begin{pmatrix} M2_{11} \\ M2_{21} \\ \end{pmatrix} $ + $ \begin{vmatrix} x2 \\ y2 \\ \end{vmatrix} $ = $ \begin{cases} x2=M2_{11}+x2 \\ y2=M2_{21}+y2 \\ \end{cases} $

</center>

3. 然后通过第一步和第二步的结果可以得到一个方程组：

$$\begin{cases}
x2=M1_{11}x1+M1_{12}y1+M2_{11} \\
y2=M1_{21}x1+M1_{22}y1+M2_{21} \\
\end{cases}$$

4. 最后把这个方程组转换成坐标点V1和一个矩阵的乘法运算：

<center>

$ \begin{vmatrix} x2 \\ y2 \\ \end{vmatrix} $ = $ \begin{pmatrix} M1_{11} & M1_{12} & M2_{11} \\ M1_{21} & M1_{22} & M2_{21} \\ \end{pmatrix} $ $ \begin{vmatrix} x1 \\ y1 \\ 1 \\ \end{vmatrix} $

</center>

> **可以看到变换前的坐标点V1多了一个维度，那么这个多了一个维度的坐标点V1就叫做齐次坐标。
得把V1坐标点转换成齐次坐标，这样才能进行仿射变换。**


**变换规则如下图所示：**

![avatar](transform.png)