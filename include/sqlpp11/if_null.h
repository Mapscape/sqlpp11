/*
 * if_null.h
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

/**
 * \brief definition of the if_null function.
 *
 */

#ifndef SQLPP_IF_NULL_H_
#define SQLPP_IF_NULL_H_

#include <sqlpp11/basic_expression_operators.h>
#include <sqlpp11/detail/type_vector.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/value_type_fwd.h>
#include <sqlpp11/wrap_operand_fwd.h>

namespace sqlpp
{
    template<typename OperandLhs, typename OperandRhs>
    struct if_null_t:
            public expression_operators<if_null_t<OperandLhs, OperandRhs>, value_type_of<OperandRhs>>,
            public alias_operators<if_null_t<OperandLhs, OperandRhs>>
    {
        using _traits = make_traits<value_type_of<OperandRhs>, tag::is_expression, tag::is_selectable>;
        using _nodes = detail::type_vector< OperandLhs, OperandRhs>;

        /// if_null inherits its name (its alias) from its first argument.
        using _alias_t = typename OperandLhs::_alias_t;
        if_null_t( OperandLhs lhs, OperandRhs rhs)
        : left( lhs), right( rhs)
        {
        }

        if_null_t(const if_null_t&) = default;
        if_null_t(if_null_t&&) = default;
        if_null_t& operator=(const if_null_t&) = default;
        if_null_t& operator=(if_null_t&&) = default;
        ~if_null_t() = default;

        OperandLhs left;
        OperandRhs right;
    };

    template<typename Context, typename OperandLhs, typename OperandRhs>
    struct serializer_t<Context, if_null_t<OperandLhs, OperandRhs>>
    {
        using _serialize_check = serialize_check_of<Context, OperandLhs, OperandRhs>;
        using T = if_null_t<OperandLhs, OperandRhs>;

        static Context& _(const T& t, Context& context)
        {
            context << "IF_NULL(";
            serialize_operand(t.left, context);
            context << ',';
            serialize_operand(t.right, context);
            context << ')';
            return context;
        }
    };

    /**
     * usage: if_null( expression, alternative)
     * The type of if_null is equal to the type of the second argument, while the default name is equal to that of the first argument.
     * The result value of this expression is whatever the underlying database returns if its IFNULL() function is used.
     */
    template< typename OperandLhs, typename OperandRhs>
    auto if_null( OperandLhs left, OperandRhs right) -> if_null_t< wrap_operand_t<OperandLhs>, wrap_operand_t<OperandRhs>>
    {
        static_assert(is_expression_t<wrap_operand_t<OperandLhs>>::value, "if_null() first argument should be an expression");

        return { left, right};
    }
}
#endif /* SQLPP_IF_NULL_H_ */
