/*
 * Copyright (C) 2019 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 12/19/2019.
 */

#ifndef SSBL_TRACES_H
#define SSBL_TRACES_H

#ifdef SSBL_YES_TRACE

#include "stdio.h"
#define SSBL_TRACE(fmt, ...) \
    printf(fmt "\r\n", ##__VA_ARGS__)
    /*
     * The double # prefix of __VA_ARGS__ allows to remove oma in case of __VA_ARGS__ is empty.
     * With the riscv toolchain, __VA_OPT__ is not supported.
     * printf(fmt "\r\n" __VA_OPT__(,) __VA_ARGS__)
     */

    #else
#define SSBL_TRACE(fmt, ...)
#endif


#endif //SSBL_TRACES_H
