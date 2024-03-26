package main

import (
  //"image"
  "fmt"
  "os"
)

var imgWidth int
var imgHeight int

func main() {
  args := os.Args
  if len(args) < 2 {
    fmt.Println("Usage:", args[0], "[image].pxs")
    return
  }
  target := args[1]
  file, err := os.ReadFile(target)
  if err != nil {
    fmt.Println("Usage:", args[0], "[image].pxs")
    return
  }
  var test uint8 = file[0]
  fmt.Println(test)
  renderImage(file)
}

func renderImage(data []byte) {
  for i := 0; i < 4; i++ {
    imgWidth = imgWidth << 8 | int(data[i])
    imgHeight = imgHeight << 8 | int(data[i+4])
  }
  fmt.Println(imgWidth, imgHeight)

  for i := 8; i < len(data); i++ {

  }
}

