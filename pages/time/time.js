// pages/time/time.js
const devicesId = "576794744" // 填写在OneNet上获得的devicesId 形式就是一串数字 例子:9939133
const api_key = "vP1UP9RJKKVWHt01rBhDyjDXPKU=" // 填写在OneNet上的 api-key 例子: VeFI0HZ44Qn5dZO14AuLbWSlSlI=
Page({
  data: {
    text:"学习了不少时间了，站起来活动一下吧。",
    time:0
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
      url: `https://api.heclouds.com/devices/${devicesId}/datapoints?datastream_id=Time&limit=1`,
      header: {
        'content-type': 'application/json',
        'api-key': api_key
      },
      success: function (res) {
        console.log(res.data);
        _this.setData(
          {
            time: (res.data.data.datastreams[0].datapoints[0].value),
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