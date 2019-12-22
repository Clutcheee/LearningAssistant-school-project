const devicesId = "576794744" // 填写在OneNet上获得的devicesId 形式就是一串数字 例子:9939133
const api_key = "vP1UP9RJKKVWHt01rBhDyjDXPKU=" // 填写在OneNet上的 api-key 例子: VeFI0HZ44Qn5dZO14AuLbWSlSlI=

Page({
  data: {
    temperature:0,
    humidity:0,
    light:0,
    DB:0,
    warn:600,
    text: "当前灯光太暗，不适合学习。为了眼睛的健康，请打开台灯或另寻场所进行学习。",
    animation: null,
    timer: null,
    duration: 0,
    textWidth: 0,
    wrapWidth: 0
  },

  onShow() {
    this.initAnimation(this.data.text)
  },
  onHide() {
    this.destroyTimer()
    this.setData({
      timer: null
    })
  },
  onUnload() {
    this.destroyTimer()
    this.setData({
      timer: null
    })
  },
  destroyTimer() {
    if (this.data.timer) {
      clearTimeout(this.data.timer);
    }
  },
  /**
   * 开启公告字幕滚动动画
   * @param  {String} text 公告内容
   * @return {[type]} 
   */
  initAnimation(text) {
    let that = this
    this.data.duration = 15000
    this.data.animation = wx.createAnimation({
      duration: this.data.duration,
      timingFunction: 'linear'
    })
    let query = wx.createSelectorQuery()
    query.select('.content-box').boundingClientRect()
    query.select('#text').boundingClientRect()
    query.exec((rect) => {
      that.setData({
        wrapWidth: rect[0].width,
        textWidth: rect[1].width
      }, () => {
        this.startAnimation()
      })
    })
  },

  // 定时器动画
  startAnimation() {
    //reset
    // this.data.animation.option.transition.duration = 0
    const resetAnimation = this.data.animation.translateX(this.data.wrapWidth).step({ duration: 0 })
    this.setData({
      animationData: resetAnimation.export()
    })
    // this.data.animation.option.transition.duration = this.data.duration
    const animationData = this.data.animation.translateX(-this.data.textWidth).step({ duration: this.data.duration })
    setTimeout(() => {
      this.setData({
        animationData: animationData.export()
      })
    }, 100)
    const timer = setTimeout(() => {
      this.startAnimation()
    }, this.data.duration)
    this.setData({
      timer
    })
  },


  onPullDownRefresh: function () {
    wx.showNavigationBarLoading()      
    setTimeout(function () {            
      wx.hideNavigationBarLoading()       
      wx.stopPullDownRefresh()      
    }, 1500);     
    if (getCurrentPages().length != 0) {
     getCurrentPages()[getCurrentPages().length - 1].onLoad()     
     }
  },   


  onLoad: function () {
    console.log(`your deviceId: ${devicesId}, apiKey: ${api_key}`)
    var _this = this;
    wx.request({
      url: `https://api.heclouds.com/devices/${devicesId}/datapoints?datastream_id=Light,Temperature,Humidity,DB&limit=1`,
      header: {
        'content-type': 'application/json',
        'api-key': api_key
      },
      success: function (res) {
        console.log(res.data);
        _this.setData(
          {
            temperature: res.data.data.datastreams[0].datapoints[0].value,
            light: res.data.data.datastreams[1].datapoints[0].value,
            humidity: res.data.data.datastreams[2].datapoints[0].value,
            DB: res.data.data.datastreams[3].datapoints[0].value
          });
      },
      fail: function () {
        wx.showToast({
          title: '与服务器通信失败',
          icon: 'fail',
          duration: 2000
        })
      }
    })
  },


})