/*
 * make_traits_tuple.h
 *
 * Copyright (c) 2015 Mapscape B.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP11_MAKE_TRAITS_TUPLE_H_
#define SQLPP11_MAKE_TRAITS_TUPLE_H_
#include <tuple>
namespace sqlpp {
/**
 * This function is a simplified version of std::make_tuple that is used
 * here because C++11's make_tuple is not constexpr yet. This allows types
 * with specific traits to create a tuple with traits-specific values, for example
 * \code
 * struct some_column {
 *      static constexpr auto traits_values = sqlpp::make_traits_tuple( can_be_null{}, has_default(0));
 *      // ...
 * \endcode
 */
template< typename... Traits>
constexpr std::tuple< Traits...> make_traits_tuple( Traits&&... traits)
{
    return std::tuple<Traits...>( std::forward<Traits>( traits)...);
}
}

#endif /* SQLPP11_MAKE_TRAITS_TUPLE_H_ */
