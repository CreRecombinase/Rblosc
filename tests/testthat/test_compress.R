context("compression/decompression")
library(Rblosc)
library(testthat)
#test_that("can compress and decompress numeric vectors", {
  
gctorture2(1,inhibit_release = TRUE)
    tvec <- rep(1.3,15)
    ttvec <- tvec+0
    ttvec[0] <- 1
    ttvec[0] <- 1.3
    res_1 <- Rblosc::bloscCompressImpl(tvec,1)
    gc()
    expect_equal(tvec,ttvec)
    res_2 <- Rblosc::bloscCompressImpl(ttvec,1)
    gc()
    expect_equal(res_1,res_2)
  checkres <- Rblosc::bloscDecompressImpl(res,length(tvec))
  
  
})
