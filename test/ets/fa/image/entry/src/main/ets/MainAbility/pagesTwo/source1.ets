// @ts-nocheck
import router from '@ohos.router';
import multimedia_image from '@ohos.multimedia.image';
import resourceManager from '@ohos.resourceManager';
@Entry
@Component
struct Index1 {
  @State imgSource: multimedia_image.ImageSource = undefined
  @State _pixelMap: multimedia_image.PixelMap = undefined

  aboutToAppear() {
    this.imgSource = multimedia_image.createImageSource('resources/base/media/2.png')
    if (this.imgSource == undefined) {
      console.info('q_y createImageSource error')
    }
  }

  btn() {
    let decodingOptions = {
      editable: true,
      desiredPixelFormat: 3
    }

    this.imgSource.createPixelMap(decodingOptions).then(async (pixelMap) => {
      this._pixelMap = pixelMap
    }).catch(error => {
      this.error = error
      console.log('q_y pixelMap error' + error);
    })
  }

  build() {
    Row() {
      Column() {
        Button('提交').onClick(() => {
          this.btn()
        }).height('10%')
          .width('15%')
        Flex({ justifyContent: FlexAlign.SpaceAround }) {
          Image($r("app.media.2"))
            .border({ width: 1, color: Color.Blue })
            .width(500)
            .height(500)
            .objectFit(ImageFit.None)
          Image(this._pixelMap).border({ width: 1, color: Color.Blue }).width(500).height(500).objectFit(ImageFit.None).backgroundColor(Color.Red)
        }

        Button() {
          Text('Back')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .backgroundColor('#0D9FFB')
        .width('40%')
        .height('5%')
        // 返回按钮绑定onClick事件，点击按钮时返回到第一页
        .onClick(() => {
          router.back()
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}