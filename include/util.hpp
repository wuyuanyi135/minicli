//
// Created by wuyua on 2/1/2022.
//

#ifndef MINICLI_UTIL_H_
#define MINICLI_UTIL_H_

#ifdef MINICLI_ENABLE_EXCEPTION
#define MINICLI_THROW_OR_RETURN(ex, ret) throw(ex)
#else
#define MINICLI_THROW_OR_RETURN(ex, ret) return (ret)
#endif


#endif  // MINICLI_UTIL_H_
