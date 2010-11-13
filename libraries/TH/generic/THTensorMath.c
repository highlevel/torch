#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/THTensorMath.c"
#else

void THTensor_(fill)(THTensor *tensor, real value)
{
  TH_TENSOR_APPLY(real, tensor, *tensor_p = value;);
}

void THTensor_(zero)(THTensor *tensor)
{
  TH_TENSOR_APPLY(real, tensor, *tensor_p = 0;);
}

void THTensor_(add)(THTensor *tensor, real value)
{
  TH_TENSOR_APPLY(real, tensor, *tensor_p += value;);
}

void THTensor_(addTensor)(THTensor *tensor, real value, THTensor *src)
{
  /* we use a trick here. careful with that. */
  TH_TENSOR_APPLY2(real, tensor, real, src,
                   long sz = (tensor_size-tensor_i < src_size-src_i ? tensor_size-tensor_i : src_size-src_i);
                   THBlas_(axpy)(sz, value, src_p, src_stride, tensor_p, tensor_stride);
                   tensor_i += sz;
                   src_i += sz;
                   tensor_p += sz*tensor_stride;
                   src_p += sz*src_stride; 
                   break;);
}

void THTensor_(mul)(THTensor *tensor, real value)
{
  /* we use a trick here. careful with that. */
  /* we do not have to increment stuff with this version (contrary to APPLY2 and APPLY3) */
  TH_TENSOR_APPLY(real, tensor, THBlas_(scal)(tensor_size, value, tensor_p, tensor_stride); break;);
}

void THTensor_(cmul)(THTensor *tensor, THTensor *src)
{
  TH_TENSOR_APPLY2(real, tensor, real, src, *tensor_p *= *src_p;);
}

void THTensor_(addcmul)(THTensor *tensor, real value, THTensor *src1, THTensor *src2)
{
  TH_TENSOR_APPLY3(real, tensor, real, src1, real, src2, *tensor_p += value * *src1_p * *src2_p;);
}

void THTensor_(div)(THTensor *tensor, real value)
{
  THArgCheck(value != 0, 2, "division by 0");
  /* we use a trick here. careful with that. */
  /* we do not have to increment stuff with this version (contrary to APPLY2 and APPLY3) */
  TH_TENSOR_APPLY(real, tensor, THBlas_(scal)(tensor_size, 1/value, tensor_p, tensor_stride); break;);
}

void THTensor_(cdiv)(THTensor *tensor, THTensor *src)
{
  TH_TENSOR_APPLY2(real, tensor, real, src, *tensor_p /= *src_p;);
}

void THTensor_(addcdiv)(THTensor *tensor, real value, THTensor *src1, THTensor *src2)
{
  TH_TENSOR_APPLY3(real, tensor, real, src1, real, src2, *tensor_p += value * *src1_p / *src2_p;);
}

real THTensor_(dot)(THTensor *tensor, THTensor *src)
{
  real sum = 0;
  /* we use a trick here. careful with that. */
  TH_TENSOR_APPLY2(real, tensor, real, src,
                   long sz = (tensor_size-tensor_i < src_size-src_i ? tensor_size-tensor_i : src_size-src_i);
                   sum += THBlas_(dot)(sz, src_p, src_stride, tensor_p, tensor_stride);
                   tensor_i += sz;
                   src_i += sz;
                   tensor_p += sz*tensor_stride;
                   src_p += sz*src_stride; 
                   break;);
  return sum; 
}

real THTensor_(min)(THTensor *tensor)
{
  real theMin = THTensor_(get1d)(tensor, 0);
  TH_TENSOR_APPLY(real, tensor, if(*tensor_p < theMin) theMin = *tensor_p;);
  return theMin; 
}

real THTensor_(max)(THTensor *tensor)
{
  real theMax = THTensor_(get1d)(tensor, 0);
  TH_TENSOR_APPLY(real, tensor, if(*tensor_p > theMax) theMax = *tensor_p;);
  return theMax; 
}

real THTensor_(sum)(THTensor *tensor)
{
  real sum = 0;
  TH_TENSOR_APPLY(real, tensor, sum += *tensor_p;);
  return sum;
}

#define TENSOR_IMPLEMENT_BASIC_FUNCTION(NAME, CFUNC)              \
  void THTensor_(NAME)(THTensor *tensor)                          \
  {                                                               \
    TH_TENSOR_APPLY(real, tensor, *tensor_p = CFUNC(*tensor_p);); \
  }

#define TENSOR_IMPLEMENT_BASIC_FUNCTION_VALUE(NAME, CFUNC)              \
  void THTensor_(NAME)(THTensor *tensor, real value)                    \
  {                                                                     \
    TH_TENSOR_APPLY(real, tensor, *tensor_p = CFUNC(*tensor_p, value);); \
  }

TENSOR_IMPLEMENT_BASIC_FUNCTION(log, log)
TENSOR_IMPLEMENT_BASIC_FUNCTION(log1p, log1p)
TENSOR_IMPLEMENT_BASIC_FUNCTION(exp, exp)
TENSOR_IMPLEMENT_BASIC_FUNCTION(cos, cos)
TENSOR_IMPLEMENT_BASIC_FUNCTION(acos, acos)
TENSOR_IMPLEMENT_BASIC_FUNCTION(cosh, cosh)
TENSOR_IMPLEMENT_BASIC_FUNCTION(sin, sin)
TENSOR_IMPLEMENT_BASIC_FUNCTION(asin, asin)
TENSOR_IMPLEMENT_BASIC_FUNCTION(sinh, sinh)
TENSOR_IMPLEMENT_BASIC_FUNCTION(tan, tan)
TENSOR_IMPLEMENT_BASIC_FUNCTION(atan, atan)
TENSOR_IMPLEMENT_BASIC_FUNCTION(tanh, tanh)
TENSOR_IMPLEMENT_BASIC_FUNCTION_VALUE(pow, pow)
TENSOR_IMPLEMENT_BASIC_FUNCTION(sqrt, sqrt)
TENSOR_IMPLEMENT_BASIC_FUNCTION(ceil, ceil)
TENSOR_IMPLEMENT_BASIC_FUNCTION(floor, floor)
TENSOR_IMPLEMENT_BASIC_FUNCTION(abs, fabs)


/* basic statistics */
real THTensor_(mean)(THTensor *tensor)
{ 
  THArgCheck(tensor->nDimension > 0, 1, "empty Tensor");
  return THTensor_(sum)(tensor)/THTensor_(nElement)(tensor);
}  

real THTensor_(var)(THTensor *tensor)
{ 
  real mean = THTensor_(mean)(tensor);
  real sum = 0;
  TH_TENSOR_APPLY(real, tensor, sum += (*tensor_p - mean)*(*tensor_p - mean););
  sum /= (THTensor_(nElement)(tensor)-1);
  return sum;
}

real THTensor_(std)(THTensor *tensor)
{ 
  return sqrt(THTensor_(var)(tensor));
} 
 
real THTensor_(norm)(THTensor *tensor, real value)
{ 
  real sum = 0;
  TH_TENSOR_APPLY(real, tensor, sum += pow(fabs(*tensor_p), value););
  return pow(sum, 1.0/value);
}

real THTensor_(dist)(THTensor *tensor, THTensor *src, real value)
{ 
  real sum = 0;
  TH_TENSOR_APPLY2(real, tensor, real, src, 
	sum += pow(fabs(*tensor_p - *src_p), value);)
  return pow(sum, 1.0/value);
}

void THTensor_(addmv)(THTensor *tensor, real alpha, THTensor *mat, THTensor *vec) 
{
  if( (mat->nDimension != 2) || (vec->nDimension != 1) )
    THError("matrix and vector expected");
 
  if( mat->size[1] != vec->size[0] )
    THError("size mismatch");

  if(tensor->nDimension != 1)
    THError("size mismatch");
    
  if( tensor->size[0] != mat->size[0] )
    THError("size mismatch");

  if(mat->stride[0] == 1)
    THBlas_(gemv)('n', mat->size[0], mat->size[1],
                  alpha, THTensor_(data)(mat), mat->stride[1],
                  THTensor_(data)(vec), vec->stride[0],
                  1, THTensor_(data)(tensor), tensor->stride[0]);

  else if(mat->stride[1] == 1)
    THBlas_(gemv)('t',  mat->size[1], mat->size[0],
                  alpha, THTensor_(data)(mat), mat->stride[0],
                  THTensor_(data)(vec), vec->stride[0],
                  1, THTensor_(data)(tensor), tensor->stride[0]);

  else
  {
    THTensor *cmat;

    THTensor_(transpose)(mat, 0, 1);
    cmat = THTensor_(newContiguous)(mat);
    THTensor_(transpose)(mat, 0, 1);

    THBlas_(gemv)('t',  mat->size[1], mat->size[0],
                  alpha, THTensor_(data)(cmat), cmat->stride[1],
                  THTensor_(data)(vec), vec->stride[0],
                  1, THTensor_(data)(tensor), tensor->stride[0]);
    
    THTensor_(free)(cmat);
  }
}

void THTensor_(addr)(THTensor *tensor, real alpha, THTensor *vec1, THTensor *vec2)
{
  if( (vec1->nDimension != 1) || (vec2->nDimension != 1) )
    THError("vector and vector expected");

  if(tensor->nDimension != 2)
    THError("size mismatch");
    
  if( (tensor->size[0] != vec1->size[0]) || (tensor->size[1] != vec2->size[0]) )
    THError("size mismatch");

  if(tensor->stride[0] == 1)
  {
    THBlas_(ger)(vec1->size[0], vec2->size[0],
                 alpha, THTensor_(data)(vec1), vec1->stride[0],
                 THTensor_(data)(vec2), vec2->stride[0],
                 THTensor_(data)(tensor), tensor->stride[1]);
  }
  else if(tensor->stride[1] == 1)
  {
    THBlas_(ger)(vec2->size[0], vec1->size[0],
                 alpha, THTensor_(data)(vec2), vec2->stride[0],
                 THTensor_(data)(vec1), vec1->stride[0],
                 THTensor_(data)(tensor), tensor->stride[0]);
  }
  else
  {
    THTensor *ctensor = THTensor_(newContiguous)(tensor);

    THBlas_(ger)(vec2->size[0], vec1->size[0],
                 alpha, THTensor_(data)(vec2), vec2->stride[0],
                 THTensor_(data)(vec1), vec1->stride[0],
                 THTensor_(data)(ctensor), ctensor->stride[0]);

    THTensor_(copy)(tensor, ctensor);
    THTensor_(free)(ctensor);
  }
}

void THTensor_(addmm)(THTensor *tensor, real alpha, THTensor *m1, THTensor *m2) 
{ 
  long r, c;
  char transpose, transpose_m1, transpose_m2;
  THTensor *tensor_, *m1_, *m2_;

  if( (m1->nDimension != 2) || (m2->nDimension != 2) ) 
    THError("matrix and matrix expected"); 
 
  if(tensor->nDimension != 2)
    THError("size mismatch"); 

  if( (tensor->size[0] != m1->size[0]) || (tensor->size[1] != m2->size[1]) || (m1->size[1] != m2->size[0]) ) 
    THError("size mismatch"); 

  /* tensor */
  if(tensor->stride[0] == 0)
  {
    transpose = 'n';
    tensor_ = tensor;
  }
  else if(tensor->stride[1] == 1)
  {
    THTensor *swap = m2;
    m2 = m1;
    m1 = swap;
    THTensor_(transpose)(tensor, 0, 1);
    THTensor_(transpose)(m1, 0, 1);
    THTensor_(transpose)(m2, 0, 1);
    transpose = 't';
    tensor_ = tensor;
  }
  else
  {
    transpose = 'n';
    tensor_ = THTensor_(newContiguous)(tensor);
  }

  /* m1 */
  if(m1->stride[0] == 1)
  {
    transpose_m1 = 'n';
    m1_ = m1;
  }
  else if(m1->stride[1] == 1)
  {
    transpose_m1 = 't';
    m1_ = m1;
  }
  else
  {
    transpose_m1 = 'n';
    m1_ = THTensor_(newContiguous)(m1);
  }

  /* m2 */
  if(m2->stride[0] == 1)
  {
    transpose_m2 = 'n';
    m2_ = m2;
  }
  else if(m2->stride[1] == 1)
  {
    transpose_m2 = 't';
    m2_ = m2;
  }
  else
  {
    transpose_m2 = 'n';
    m2_ = THTensor_(newContiguous)(m2);
  }

  /* do the operation */
  THBlas_(gemm)(transpose_m1, transpose_m2, tensor_->size[0], tensor_->size[1], m1_->size[1], alpha,
                THTensor_(data)(m1_), m1_->stride[1],
                THTensor_(data)(m2_), m2_->stride[1],
                1, THTensor_(data)(tensor_), tensor_->stride[1]);

  /* free intermediate variables */
  if(m1_ != m1)
    THTensor_(free)(m1_);

  if(m2_ != m2)
    THTensor_(free)(m2_);

  if(tensor_ != tensor)
    THTensor_(free)(tensor_);

  if(transpose == 't')
  {
    THTensor_(transpose)(tensor, 0, 1);
    THTensor_(transpose)(m1, 0, 1);
    THTensor_(transpose)(m2, 0, 1);
  }
} 

#endif