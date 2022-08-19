// index.ets
import router from '@ohos.router';

@Entry
@Component
struct Index {
  @State message: string = 'ImageTest'

  build() {
    Row() {
      Column() {
        Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.SpaceEvenly }){
          Text(this.message)
            .fontSize(50)
            .fontWeight(FontWeight.Bold)
          // 添加按钮，以响应用户点击
          Button() {
            Text('PixelMap')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/PixelMap' })
          })

          Button() {
            Text('ImageSource')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/ImageSource' })
          })

          Button() {
            Text('ImagePacker')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/ImagePacker' })
          })

          Button() {
            Text('ImageReceiver')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/ImageReceiver' })
          })

          Button() {
            Text('Image')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/Image' })
          })

          Button() {
            Text('source1')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/source1' })
          })

          Button() {
            Text('camera')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/CameraPage' })
          })

          Button() {
            Text('index1')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
          }
          .type(ButtonType.Capsule)
          .margin({
            top: 20
          })
          .backgroundColor('#0D9FFB')
          .width('60%')
          .height('5%')
          // 跳转按钮绑定onClick事件，点击时跳转到第二页
          .onClick(() => {
            router.push({ url: 'pagesTwo/index1' })
          })
        }

      }
      .width('100%')
    }
    .height('100%')
  }
}
