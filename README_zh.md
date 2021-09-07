# Image组件<a name="ZH-CN_TOPIC_0000001139841951"></a>

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [说明](#section1312121216216)
    -   [接口说明](#section1551164914237)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1533973044317)

## 简介<a name="section11660541593"></a>

Image组件提供了简单易用的接口用于开发图像的编解码功能，目前支持主流的如下图片格式：JPEG、PNG、BMP、WEBP、GIF、HEIF、RAW。

**图 1**  Image组件架构图<a name="fig99659301300"></a>  
![](figures/Image组件架构图.png "Image组件架构图")

## 目录<a name="section161941989596"></a>

仓目录结构如下：

```
/foundation/multimedia/image      # Image组件业务代码
├── adapter                       # 适配层代码
│   └── frameworks                # 框架适配层
│   └── hals                      # hal适配层
├── frameworks                    # 框架代码
│   ├── innerkitsimpl             # 框架native层业务实现
│   └── jni                       # jni层实现
├── ohos.build                    # 编译配置
├── interfaces                    # 外部接口层
│   ├── innerkits                 # 内部其他子系统接口
│   ├── kits                      # java接口层
│   └── native                    # ndk接口层
├── mock                          # mock实现
├── plugins                       # 图像插件实现
│   ├── common                    # 图像通用插件
│   └── manager                   # 图像插件管理模块
├── test                          # 测试资源目录
```

## 说明<a name="section1312121216216"></a>

### 接口说明<a name="section1551164914237"></a>

ImageSource提供的主要接口如下：

<a name="table186664684917"></a>
<table><thead align="left"><tr id="row886615466490"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p1786684619498"><a name="p1786684619498"></a><a name="p1786684619498"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p208661246174919"><a name="p208661246174919"></a><a name="p208661246174919"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row11866164604912"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6296135817496"><a name="p6296135817496"></a><a name="p6296135817496"></a>create(String pathName, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p41486101504"><a name="p41486101504"></a><a name="p41486101504"></a>从图像文件路径创建图像数据源。</p>
</td>
</tr>
<tr id="row17866346184917"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p142961058124919"><a name="p142961058124919"></a><a name="p142961058124919"></a>create(InputStream is, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p2149141013505"><a name="p2149141013505"></a><a name="p2149141013505"></a>从输入流创建图像数据源。</p>
</td>
</tr>
<tr id="row1786774614912"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p14296458184916"><a name="p14296458184916"></a><a name="p14296458184916"></a>create(byte[] data, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p151491410145020"><a name="p151491410145020"></a><a name="p151491410145020"></a>从字节数组创建图像源。</p>
</td>
</tr>
<tr id="row78672046184918"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p15296958164917"><a name="p15296958164917"></a><a name="p15296958164917"></a>create(byte[] data, int offset, int length, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1014991017507"><a name="p1014991017507"></a><a name="p1014991017507"></a>从字节数组指定范围创建图像源。</p>
</td>
</tr>
<tr id="row10867174694916"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p12961158174913"><a name="p12961158174913"></a><a name="p12961158174913"></a>create(File file, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p2014951020504"><a name="p2014951020504"></a><a name="p2014951020504"></a>从文件对象创建图像数据源。</p>
</td>
</tr>
<tr id="row8867194617492"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p19296135816496"><a name="p19296135816496"></a><a name="p19296135816496"></a>create(FileDescriptor fd, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16149010165018"><a name="p16149010165018"></a><a name="p16149010165018"></a>从文件描述符创建图像数据源。</p>
</td>
</tr>
<tr id="row818555010490"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p152961358144910"><a name="p152961358144910"></a><a name="p152961358144910"></a>createIncrementalSource(SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p0149151020508"><a name="p0149151020508"></a><a name="p0149151020508"></a>创建渐进式图像数据源。</p>
</td>
</tr>
<tr id="row14427205054917"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6296158144918"><a name="p6296158144918"></a><a name="p6296158144918"></a>createIncrementalSource(IncrementalSourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p014916103500"><a name="p014916103500"></a><a name="p014916103500"></a>创建渐进式图像数据源，支持设置渐进式数据更新模式。</p>
</td>
</tr>
<tr id="row169865012496"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p42961958154910"><a name="p42961958154910"></a><a name="p42961958154910"></a>createPixelmap(DecodingOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1015031010503"><a name="p1015031010503"></a><a name="p1015031010503"></a>从图像数据源解码并创建PixelMap图像。</p>
</td>
</tr>
<tr id="row12867144611493"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p3296105817491"><a name="p3296105817491"></a><a name="p3296105817491"></a>createPixelmap(int index, DecodingOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p91503102508"><a name="p91503102508"></a><a name="p91503102508"></a>从图像数据源解码并创建PixelMap图像，如果图像数据源支持多张图片的话，支持指定图像索引。</p>
</td>
</tr>
<tr id="row73286115501"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p19296658194916"><a name="p19296658194916"></a><a name="p19296658194916"></a>release()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p115091015020"><a name="p115091015020"></a><a name="p115091015020"></a>释放对象关联的本地资源。</p>
</td>
</tr>
</tbody>
</table>

PixelMap提供的主要接口如下：

<a name="table16114406561"></a>
<table><thead align="left"><tr id="row61149016568"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p61141104567"><a name="p61141104567"></a><a name="p61141104567"></a>接口名</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p211410018561"><a name="p211410018561"></a><a name="p211410018561"></a>描述</p>
</th>
</tr>
</thead>
<tbody><tr id="row16114208563"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p67216918565"><a name="p67216918565"></a><a name="p67216918565"></a>create(InitializationOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p439991595617"><a name="p439991595617"></a><a name="p439991595617"></a>根据图像大小、像素格式、alpha类型等初始化选项创建PixelMap。</p>
</td>
</tr>
<tr id="row181141909564"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p17212910567"><a name="p17212910567"></a><a name="p17212910567"></a>create(PixelMap source, Rect srcRegion, InitializationOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p113991415195617"><a name="p113991415195617"></a><a name="p113991415195617"></a>根据图像大小、像素格式、alpha类型等初始化选项,以源PixelMap、源裁剪区域描述的数据源创建PixelMap。</p>
</td>
</tr>
<tr id="row19115407566"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1772113955616"><a name="p1772113955616"></a><a name="p1772113955616"></a>getBytesNumberPerRow()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16400715175615"><a name="p16400715175615"></a><a name="p16400715175615"></a>获取每行像素数据占用的字节数。</p>
</td>
</tr>
<tr id="row161151109562"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6721119165612"><a name="p6721119165612"></a><a name="p6721119165612"></a>getPixelBytesCapacity()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p2040031515614"><a name="p2040031515614"></a><a name="p2040031515614"></a>获取存储Pixelmap像素数据的内存容量。</p>
</td>
</tr>
<tr id="row611510165617"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p117211293566"><a name="p117211293566"></a><a name="p117211293566"></a>readPixel(Position pos)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p19400111535612"><a name="p19400111535612"></a><a name="p19400111535612"></a>读取指定位置像素的颜色值,返回的颜色格式为PixelFormat.ARGB_8888。</p>
</td>
</tr>
<tr id="row811519015564"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1872239185610"><a name="p1872239185610"></a><a name="p1872239185610"></a>readPixels(int[] pixels, int offset, int stride, Rect region)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p540071565613"><a name="p540071565613"></a><a name="p540071565613"></a>读取指定区域像素的颜色值,输出到以起始偏移量、行像素大小描述的像素数组，返回的颜色格式为PixelFormat.ARGB_8888。</p>
</td>
</tr>
<tr id="row1111510135612"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p107229918561"><a name="p107229918561"></a><a name="p107229918561"></a>readPixels(Buffer dst)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p640051512562"><a name="p640051512562"></a><a name="p640051512562"></a>读取像素的颜色值到缓冲区,返回的数据是PixelMap中像素数据的原样拷贝，即返回的颜色数据格式与PixelMap中像素格式一致。</p>
</td>
</tr>
<tr id="row164761121569"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p17221593562"><a name="p17221593562"></a><a name="p17221593562"></a>writePixels(int[] pixels, int offset, int stride, Rect region)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p134001515105617"><a name="p134001515105617"></a><a name="p134001515105617"></a>将像素颜色数组、起始偏移量、行像素的个数描述的源像素数据写入PixelMap的指定区域,写入颜色格式为PixelFormat.ARGB_8888。</p>
</td>
</tr>
<tr id="row187170245618"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p2072211912562"><a name="p2072211912562"></a><a name="p2072211912562"></a>writePixels(Buffer src)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1740012155566"><a name="p1740012155566"></a><a name="p1740012155566"></a>将缓冲区描述的源像素数据写入PixelMap,写入的数据将原样覆盖PixelMap中的像素数据，即写入数据的颜色格式应与PixelMap的配置兼容。</p>
</td>
</tr>
<tr id="row11881927561"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1972213910566"><a name="p1972213910566"></a><a name="p1972213910566"></a>getImageInfo()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1740016157566"><a name="p1740016157566"></a><a name="p1740016157566"></a>获取图像基本信息。</p>
</td>
</tr>
<tr id="row424111385619"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1172210917564"><a name="p1172210917564"></a><a name="p1172210917564"></a>release()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p64005156563"><a name="p64005156563"></a><a name="p64005156563"></a>释放对象关联的本地资源。</p>
</td>
</tr>
</tbody>
</table>

### 使用说明<a name="section129654513264"></a>

下面提供了使用ImageSource解析本地图片的例子：

```
ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
srcOpts.formatHint = "image/png";
String pathName = "/sdcard/image.png";
ImageSource imageSource = ImageSource.create(pathName, srcOpts);
ImageSource imageSourceNoOptions = ImageSource.create(pathName, null);
// 普通解码叠加缩放、裁剪、旋转
ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
decodingOpts.desiredSize = new Size(100, 2000);
decodingOpts.desiredRegion = new Rect(0, 0, 100, 100);
decodingOpts.rotateDegrees = 90;
PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
// 普通模式解码
PixelMap pixelMapNoOptions = imageSource.createPixelmap(null);
```

## 相关仓<a name="section1533973044317"></a>

hmf/multimedia/image

