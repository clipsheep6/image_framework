# Image<a name="EN-US_TOPIC_0000001139841951"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Usage](#section1312121216216)
    -   [Available APIs](#section1551164914237)
    -   [Usage Guidelines](#section129654513264)

-   [Repositories Involved](#section1533973044317)

## Introduction<a name="section11660541593"></a>

The  **image**  repository provides easy-to-use APIs for developing image encoding and decoding features. Currently, the following image formats are supported: JPEG, PNG, BMP, WEBP, GIF, HEIF, and RAW.

**Figure  1**  Image architecture<a name="fig99659301300"></a>  
![](figures/image-architecture.png "image-architecture")

## Directory Structure<a name="section161941989596"></a>

The structure of the repository directory is as follows:

```
/foundation/multimedia/image   
├── adapter                       # Adaptation code
│   ├── frameworks                # Framework adaptation
│   └── hals                      # HAL adaptation
├── frameworks                    # Framework code
│   ├── innerkitsimpl             # Native API implementation
│   └── jni                       # JNI implementation
├── ohos.build                    # Build configuration
├── interfaces                    # External APIs
│   ├── innerkits                 # APIs of other internal subsystems
│   ├── kits                      # Java APIs
│   └── native                    # NDK APIs
├── mock                          # Mock implementation
├── plugins                       # Image plug-in implementation
│   ├── common                    # Common image plug-ins
│   ├── manager                   # Image plug-in manager
├── test                          # Test resources
```

## Usage<a name="section1312121216216"></a>

### Available APIs<a name="section1551164914237"></a>

Major methods in  **ImageSource**

<a name="table186664684917"></a>
<table><thead align="left"><tr id="row886615466490"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p1786684619498"><a name="p1786684619498"></a><a name="p1786684619498"></a>Method</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p208661246174919"><a name="p208661246174919"></a><a name="p208661246174919"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row11866164604912"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6296135817496"><a name="p6296135817496"></a><a name="p6296135817496"></a>create(String pathName, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p41486101504"><a name="p41486101504"></a><a name="p41486101504"></a>Creates an <strong id="b1553095354415"><a name="b1553095354415"></a><a name="b1553095354415"></a>ImageSource</strong> based on an image file path.</p>
</td>
</tr>
<tr id="row17866346184917"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p142961058124919"><a name="p142961058124919"></a><a name="p142961058124919"></a>create(InputStream is, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p2149141013505"><a name="p2149141013505"></a><a name="p2149141013505"></a>Creates an <strong id="b16305135910444"><a name="b16305135910444"></a><a name="b16305135910444"></a>ImageSource</strong> based on an input stream.</p>
</td>
</tr>
<tr id="row1786774614912"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p14296458184916"><a name="p14296458184916"></a><a name="p14296458184916"></a>create(byte[] data, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p151491410145020"><a name="p151491410145020"></a><a name="p151491410145020"></a>Creates an <strong id="b761610244517"><a name="b761610244517"></a><a name="b761610244517"></a>ImageSource</strong> based on a byte array.</p>
</td>
</tr>
<tr id="row78672046184918"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p15296958164917"><a name="p15296958164917"></a><a name="p15296958164917"></a>create(byte[] data, int offset, int length, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1014991017507"><a name="p1014991017507"></a><a name="p1014991017507"></a>Creates an <strong id="b799336164513"><a name="b799336164513"></a><a name="b799336164513"></a>ImageSource</strong> based on a byte array with specified offset and length.</p>
</td>
</tr>
<tr id="row10867174694916"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p12961158174913"><a name="p12961158174913"></a><a name="p12961158174913"></a>create(File file, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p2014951020504"><a name="p2014951020504"></a><a name="p2014951020504"></a>Creates an <strong id="b375261524517"><a name="b375261524517"></a><a name="b375261524517"></a>ImageSource</strong> based on a <strong id="b1375741519456"><a name="b1375741519456"></a><a name="b1375741519456"></a>File</strong> object.</p>
</td>
</tr>
<tr id="row8867194617492"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p19296135816496"><a name="p19296135816496"></a><a name="p19296135816496"></a>create(FileDescriptor fd, SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16149010165018"><a name="p16149010165018"></a><a name="p16149010165018"></a>Creates an <strong id="b18525420104517"><a name="b18525420104517"></a><a name="b18525420104517"></a>ImageSource</strong> based on a file descriptor.</p>
</td>
</tr>
<tr id="row818555010490"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p152961358144910"><a name="p152961358144910"></a><a name="p152961358144910"></a>createIncrementalSource(SourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p0149151020508"><a name="p0149151020508"></a><a name="p0149151020508"></a>Creates an incremental data source based on a <strong id="b117291324144516"><a name="b117291324144516"></a><a name="b117291324144516"></a>SourceOptions</strong> object.</p>
</td>
</tr>
<tr id="row14427205054917"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6296158144918"><a name="p6296158144918"></a><a name="p6296158144918"></a>createIncrementalSource(IncrementalSourceOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p014916103500"><a name="p014916103500"></a><a name="p014916103500"></a>Creates an incremental data source based on an <strong id="b1685814335458"><a name="b1685814335458"></a><a name="b1685814335458"></a>IncrementalSourceOptions</strong> object.</p>
</td>
</tr>
<tr id="row169865012496"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p42961958154910"><a name="p42961958154910"></a><a name="p42961958154910"></a>createPixelmap(DecodingOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1015031010503"><a name="p1015031010503"></a><a name="p1015031010503"></a>Decodes source image data and creates a pixel map.</p>
</td>
</tr>
<tr id="row12867144611493"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p3296105817491"><a name="p3296105817491"></a><a name="p3296105817491"></a>createPixelmap(int index, DecodingOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p91503102508"><a name="p91503102508"></a><a name="p91503102508"></a>Decodes source image data based on a specified index location in the <strong id="b1902344124515"><a name="b1902344124515"></a><a name="b1902344124515"></a>ImageSource</strong> and creates a pixel map.</p>
</td>
</tr>
<tr id="row73286115501"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p19296658194916"><a name="p19296658194916"></a><a name="p19296658194916"></a>release()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p115091015020"><a name="p115091015020"></a><a name="p115091015020"></a>Releases local resources associated with the <strong id="b38419553451"><a name="b38419553451"></a><a name="b38419553451"></a>PixelMap</strong> object.</p>
</td>
</tr>
</tbody>
</table>

Major methods in  **PixelMap**

<a name="table16114406561"></a>
<table><thead align="left"><tr id="row61149016568"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p61141104567"><a name="p61141104567"></a><a name="p61141104567"></a>Method</p>
</th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p211410018561"><a name="p211410018561"></a><a name="p211410018561"></a>Description</p>
</th>
</tr>
</thead>
<tbody><tr id="row16114208563"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p67216918565"><a name="p67216918565"></a><a name="p67216918565"></a>create(InitializationOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p439991595617"><a name="p439991595617"></a><a name="p439991595617"></a>Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type).</p>
</td>
</tr>
<tr id="row181141909564"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p17212910567"><a name="p17212910567"></a><a name="p17212910567"></a>create(PixelMap source, Rect srcRegion, InitializationOptions opts)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p113991415195617"><a name="p113991415195617"></a><a name="p113991415195617"></a>Creates a pixel map based on initialization options (such as the image size, pixel format, and alpha type) and the data source described by a source pixel map and the expected area on it.</p>
</td>
</tr>
<tr id="row19115407566"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1772113955616"><a name="p1772113955616"></a><a name="p1772113955616"></a>getBytesNumberPerRow()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p16400715175615"><a name="p16400715175615"></a><a name="p16400715175615"></a>Obtains the number of bytes in each row of pixels.</p>
</td>
</tr>
<tr id="row161151109562"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p6721119165612"><a name="p6721119165612"></a><a name="p6721119165612"></a>getPixelBytesCapacity()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p2040031515614"><a name="p2040031515614"></a><a name="p2040031515614"></a>Obtains the memory capacity for storing the pixels of this pixel map.</p>
</td>
</tr>
<tr id="row611510165617"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p117211293566"><a name="p117211293566"></a><a name="p117211293566"></a>readPixel(Position pos)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p19400111535612"><a name="p19400111535612"></a><a name="p19400111535612"></a>Reads the color value at the specified position. The color value is in <strong id="b1119064411493"><a name="b1119064411493"></a><a name="b1119064411493"></a>PixelFormat.ARGB_8888</strong> format.</p>
</td>
</tr>
<tr id="row811519015564"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1872239185610"><a name="p1872239185610"></a><a name="p1872239185610"></a>readPixels(int[] pixels, int offset, int stride, Rect region)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p540071565613"><a name="p540071565613"></a><a name="p540071565613"></a>Reads the color values of a specified region and writes them into a <strong id="b121411547134914"><a name="b121411547134914"></a><a name="b121411547134914"></a>pixels</strong> array with the specified start offset and stride. The color values are in <strong id="b7142184716493"><a name="b7142184716493"></a><a name="b7142184716493"></a>PixelFormat.ARGB_8888</strong> format.</p>
</td>
</tr>
<tr id="row1111510135612"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p107229918561"><a name="p107229918561"></a><a name="p107229918561"></a>readPixels(Buffer dst)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p640051512562"><a name="p640051512562"></a><a name="p640051512562"></a>Reads a copy of color values of this <strong id="b9125155654910"><a name="b9125155654910"></a><a name="b9125155654910"></a>PixelMap</strong> instance and stores it to the specified buffer.</p>
</td>
</tr>
<tr id="row164761121569"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p17221593562"><a name="p17221593562"></a><a name="p17221593562"></a>writePixels(int[] pixels, int offset, int stride, Rect region)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p134001515105617"><a name="p134001515105617"></a><a name="p134001515105617"></a>Writes data from the specified color data array (based on the start offset and stride) into the specified region of this <strong id="b889818012508"><a name="b889818012508"></a><a name="b889818012508"></a>PixelMap</strong> instance. The color data to write is in <strong id="b390311075011"><a name="b390311075011"></a><a name="b390311075011"></a>PixelFormat.ARGB_8888</strong> format.</p>
</td>
</tr>
<tr id="row187170245618"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p2072211912562"><a name="p2072211912562"></a><a name="p2072211912562"></a>writePixels(Buffer src)</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1740012155566"><a name="p1740012155566"></a><a name="p1740012155566"></a>Writes the pixel data in the specified buffer into this <strong id="b11154713145019"><a name="b11154713145019"></a><a name="b11154713145019"></a>PixelMap</strong>. The buffer data will overwrite the <strong id="b815971335017"><a name="b815971335017"></a><a name="b815971335017"></a>PixelMap</strong> data, so the color format of the source data must be compatible with this <strong id="b01601313205019"><a name="b01601313205019"></a><a name="b01601313205019"></a>PixelMap</strong>.</p>
</td>
</tr>
<tr id="row11881927561"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1972213910566"><a name="p1972213910566"></a><a name="p1972213910566"></a>getImageInfo()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1740016157566"><a name="p1740016157566"></a><a name="p1740016157566"></a>Obtains basic image information.</p>
</td>
</tr>
<tr id="row424111385619"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1172210917564"><a name="p1172210917564"></a><a name="p1172210917564"></a>release()</p>
</td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p64005156563"><a name="p64005156563"></a><a name="p64005156563"></a>Releases local resources associated with the <strong id="b5712422185013"><a name="b5712422185013"></a><a name="b5712422185013"></a>PixelMap</strong> object.</p>
</td>
</tr>
</tbody>
</table>

### Usage Guidelines<a name="section129654513264"></a>

The following example shows how to parse a local image by calling methods in  **ImageSource**:

```
ImageSource.SourceOptions srcOpts = new ImageSource.SourceOptions();
srcOpts.formatHint = "image/png";
String pathName = "/sdcard/image.png";
ImageSource imageSource = ImageSource.create(pathName, srcOpts);
ImageSource imageSourceNoOptions = ImageSource.create(pathName, null);
// Common decoding with scaling, cropping, and rotation
ImageSource.DecodingOptions decodingOpts = new ImageSource.DecodingOptions();
decodingOpts.desiredSize = new Size(100, 2000);
decodingOpts.desiredRegion = new Rect(0, 0, 100, 100);
decodingOpts.rotateDegrees = 90;
PixelMap pixelMap = imageSource.createPixelmap(decodingOpts);
// Common decoding mode
PixelMap pixelMapNoOptions = imageSource.createPixelmap(null);
```

## Repositories Involved<a name="section1533973044317"></a>

hmf/multimedia/image

