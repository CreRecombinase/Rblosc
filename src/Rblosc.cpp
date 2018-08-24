
#include <memory>
#include <array>

#include <Rcpp.h>
#include <blosc.h>
//[[Rcpp::plugins(cpp17)]]


// [[Rcpp::export]]
Rcpp::RawVector bloscCompressImpl(Rcpp::NumericVector data, int level,std::string filter="blosclz") {

  blosc_init();
  std::vector<std::string> filters = {"blosclz", "lz4", "lz4hc", "snappy", "zlib", "zstd"};

  auto which_filt=std::find(filters.begin(),filters.end(),filter);
  if(which_filt==filters.end()){
    Rcpp::stop("Filter "+filter+"not found!");
  }
  auto rcode = blosc_set_compressor(which_filt->c_str());
  if (level < 0) {
    Rcpp::warning("compression level can't be less than 1, assuming it is 1");
    level = 0;
  }
  
  //  auto input = Rcpp::as<Rcpp::RawVector>(data);
  auto sz = data.size()*sizeof(double);
  auto packed = Rcpp::RawVector::create(sz*2);

  // std::unique_ptr<uint8_t[]> temp(new uint8_t[sz]());
  
  // auto rc = ZSTD_compress(&temp[0], sz, input.begin(), input.size(), level);
  double* rb=&data[0];
  Rbyte* wb=&packed[0];
  auto csize = blosc_compress(level,
			      1,
			      sizeof(double),
			      sz,
			      rb,
			      wb,
			      sz+BLOSC_MAX_OVERHEAD);
  if(csize<0){
    Rcpp::stop("Compression error. Error code:"+std::to_string(csize));
  }
  printf("Compression: %d -> %d (%.1fx)\n", sz, csize, (1.*sz) / csize);
  Rcpp::RawVector ret(csize);
  std::copy(packed.begin(),packed.begin()+csize,ret.begin());
  blosc_destroy();
  return ret;
}



// [[Rcpp::export]]
Rcpp::NumericVector bloscDecompressImpl(Rcpp::RawVector data,int original_size) {

  // std::vector<std::string> filters = {"blosclz", "lz4", "lz4hc", "snappy", "zlib", "zstd"};
  // auto which_filt=std::find(filters.begin(),filters.end(),filter);
  // if(which_filt==filters.end()){
  //   Rcpp::stop("Filter "+filter+"not found!");
  // }
  //  auto rcode = blosc_set_compressor(which_filt->c_str());
  auto sz = data.size();
  Rcpp::NumericVector data_dest(original_size);
  auto csize = blosc_decompress(&data[0], &data_dest[0], original_size*sizeof(double));
  if(csize<0){
    Rcpp::stop("Decompression error. Error code:"+std::to_string(csize));
  }
  Rcpp::NumericVector retv(data_dest.begin(),data_dest.begin()+csize);
  blosc_destroy();
  return(retv);
}
