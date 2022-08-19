// @ts-nocheck
import router from '@ohos.router';
import multimedia_image from '@ohos.multimedia.image';
import resourceManager from '@ohos.resourceManager';
@Entry
@Component
struct Index1 {
  @State imgSource: multimedia_image.ImageSource = undefined
  @State _pixelMap: multimedia_image.PixelMap = undefined
  async getFd(fileName) {
    let mgr = await resourceManager.getResourceManager()
    let value = await mgr.getRawFileDescriptor(fileName)
    console.info('q_y0 fd ')
    return value.fd
  }

  async aboutToAppear() {
    let fd = await this.getFd('2.png')
    console.info('q_y1 fd ' + fd)
    this.imgSource = multimedia_image.createImageSource(fd)
  }

  async btn() {
    let decodingOptions = {
      editable: true,
      desiredPixelFormat: 3
    }
    this.imgSource.createPixelMap(decodingOptions).then(async (pixelMap) => {

//      this.imgSource

      this._pixelMap = pixelMap
    }).catch(error => {
      this.error = error
      console.log('pixelMap error' + error);
    })
  }



  build() {
    Row() {
      Column() {
        Button('提交').onClick(() => {
          this.btn()
        }).height('10%')
        .width('15%')
        .margin(20)

//        Button('imageS').onClick(() => {
//          this.imageS()
//        }).height('10%')
//          .width('15%')
//        .margin(20)

        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          Image(this._pixelMap)
            .border({ width: 1, color: Color.Blue })
            .width(500).height(500)
            .objectFit(ImageFit.None)
            .backgroundColor(Color.Red)
        }
      }
      .width('100%')
    }
    .height('100%')
  }
}