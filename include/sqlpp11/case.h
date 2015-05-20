/*
 * case.h
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

#ifndef _SQLPP_CASE_H_
#define _SQLPP_CASE_H_
#include <tuple>
#include <type_traits>
#include <sqlpp11/no_value.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/expression.h>


namespace sqlpp {

    /**
     * This struct corresponds to a "WHEN a THEN b" clause inside a case statement.
     */
    template< typename WhenType, typename ThenType>
    struct when_t
    {
        WhenType _when;
        ThenType _then;
        using value_type = value_type_of<ThenType>;
    };

    /**
     * Generator function for when_t.
     * The make_when takes expressions and converts them to an operand type if needed, e.g. if an expression of
     * type 'int' is given, it will be converted to an expression type.
     *
     */
    template< typename WhenType, typename ThenType>
    when_t< wrap_operand_t<WhenType>, wrap_operand_t<ThenType>> make_when( WhenType when, ThenType then )
    {
        return {when, then};
    }

    namespace detail {

        /**
         * cat_types is a template meta function that takes a tuple-type of when_t structs and that collects the WhenType and ThenType of these
         * struct types and adds them to the type_vector Accumulator.
         */
        // if you get a compiler error here, then the "WhenTuple" type was not a tuple of only when_t types.
        template< typename WhenTuple, typename Accumulator = detail::type_vector<>>
        struct cat_types {};

        // specialization for non-empty tuple where the first element is a when_t and where the rest of the tuple
        // is assumed to be either empty or to consist solely of when_t types.
        template< typename WhenType, typename ThenType, typename... Types, typename... Tail>
        struct cat_types< std::tuple<when_t< WhenType, ThenType>, Tail...>, detail::type_vector< Types...>>
        {
            using type = typename cat_types< std::tuple< Tail...>, detail::type_vector< Types..., WhenType, ThenType>>::type;
        };

        // specialization for empty tuple to end recursion.
        template<typename Accumulator>
        struct cat_types< std::tuple<>, Accumulator>
        {
            using type = Accumulator;
        };

        template< typename Whens, typename... InitialTypes>
        using cat_types_t = typename cat_types<Whens, detail::type_vector< InitialTypes...>>::type;

        /**
         * Add a type to a type_vector if it is not no_value_t
         */
        template< typename ToAdd, typename Accumulator = detail::type_vector<>>
        struct add_if_value
        {
        };

        // generic case, append the type to the end of the vector
        template< typename ToAdd, typename... Elements>
        struct add_if_value< ToAdd, detail::type_vector< Elements...>>
        {
            using type = detail::type_vector< Elements..., ToAdd>;
        };

        // specialization for no_value_t, which does nothing.
        template< typename... Elements>
        struct add_if_value< no_value_t, detail::type_vector< Elements...>>
        {
            using type = detail::type_vector< Elements...>;
        };

        template< typename ToAdd, typename Accumulator = detail::type_vector<>>
        using add_if_value_t = typename add_if_value< ToAdd, Accumulator>::type;

        template< typename Context, typename NodeVector>
        struct serialize_check_vector {};

        template< typename Context, typename... Nodes>
        struct serialize_check_vector< Context, detail::type_vector< Nodes...>>
        {
            using type = serialize_check_of< Context, Nodes...>;
        };

        template< typename Context, typename NodeVector>
        using serialize_check_vector_t = typename serialize_check_vector< Context, NodeVector>::type;
    }

    /**
     * This struct corresponds to
     * CASE [<expr>] [WHEN <expr> THEN <expr>...] [ELSE <expr>] END
     *
     * This type enables when() and else_() member functions, but only if no else-expression has been
     * specified before. In other words, the else_() member function must be the last function call in the chain.
     */
    template< typename CaseExpression = no_value_t, typename ElseExpression = no_value_t, typename WhenTuple = std::tuple<>>
    struct case_t :
            public expression_operators<
                case_t< CaseExpression, ElseExpression, WhenTuple>,
                typename std::conditional<
                                    std::is_same< std::tuple<>, WhenTuple>::value,
                                    void,
                                    typename std::tuple_element< 0, WhenTuple>::type::value_type
                                >::type
                                >,
            public alias_operators< case_t< CaseExpression, ElseExpression, WhenTuple>>
    {
        case_t( CaseExpression ce, ElseExpression ee, WhenTuple wt)
                : _case( ce), _else( ee), _when( wt)
        {
        }

        using _traits = make_traits<
                typename std::conditional<
                    std::is_same< std::tuple<>, WhenTuple>::value,
                    void,
                    typename std::tuple_element< 0, WhenTuple>::type::value_type
                >::type,
                tag::is_expression,
                tag::is_selectable>
            ;

        using _nodes = typename detail::cat_types< WhenTuple, detail::add_if_value_t<CaseExpression, detail::add_if_value_t<ElseExpression>>>::type;

        struct _alias_t
        {
            static constexpr const char _literal[] =  "case_";
            using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
            template<typename T>
                struct _member_t
                {
                    T case_;
                    T& operator()() { return case_; }
                    const T& operator()() const { return case_; }
                };
        };


        /**
         * The when() member function is enabled only if no else-expression was specified.
         */
        template< typename WhenExpression, typename ThenExpression>
        auto when( WhenExpression when_expr, ThenExpression then)
        ->  typename std::enable_if<
                std::is_same< ElseExpression, no_value_t>::value,
                case_t< CaseExpression, ElseExpression, decltype( std::tuple_cat( WhenTuple{}, std::make_tuple(make_when(when_expr, then))))>
            >::type
        {
            return { _case, _else, tuple_cat( _when, std::make_tuple( make_when( when_expr, then)))};
        }

        /**
         * The else_() member function is enabled only if no previous else-expression was specified.
         */
        template <typename NewElseExpression>
        typename std::enable_if<
            std::is_same< ElseExpression, no_value_t>::value,
            case_t< CaseExpression, wrap_operand_t<NewElseExpression>, WhenTuple>
        >::type else_( NewElseExpression else_expression)
        {
            return { _case, else_expression, _when};
        }

        CaseExpression _case;
        ElseExpression _else;
        WhenTuple      _when;
    };

    /**
     * This struct corresponds to the fragment
     * CASE [<expr>]
     *
     * This is not a valid expression by itself. It returns a valid expression from
     * its when() member function.
     */
    template< typename CaseExpression = no_value_t>
    struct bare_case_t
    {
        template< typename WhenExpression, typename ThenExpression>
        auto when( WhenExpression when_expr, ThenExpression then)
        -> case_t< CaseExpression, no_value_t, std::tuple<when_t<wrap_operand_t<WhenExpression>, wrap_operand_t<ThenExpression>>>>
        {
            typedef case_t< CaseExpression, no_value_t, std::tuple<when_t<wrap_operand_t<WhenExpression>, wrap_operand_t<ThenExpression>>>> return_value;
            return return_value{ _case, no_value_t{}, std::make_tuple( make_when( when_expr, then))};
        }

        CaseExpression _case;
    };

    /**
     * Create a CASE-expression.
     *
     * CASE expressions in SQL have the form:
     *     CASE [<expression>] [WHEN <w-expr> THEN <t-expr>]... [ELSE <e-expr>] END
     *
     * which is expressed through this function in the following form:
     *     case_( <expression>)
     *         .when( <w-expr>, <t-expr>)
     *         .when( <w-expr>, <t-expr>)
     *         .else_( <e-expr>)
     *         // etc...
     *
     * There is an overload of this function that takes no arguments. In that
     * case the <w-expr> expressions are expected to be of some boolean type.
     *
     * Take note of the trailing underscore in the names case_ and else_. Unfortunately
     * "case" and "else" are both SQL and C++ keywords.
     */
    template< typename CaseExpression>
    bare_case_t<wrap_operand_t<CaseExpression>> case_( CaseExpression expr)
    {
        return { expr};
    }

    /**
     * \overload
     */
    inline bare_case_t<> case_()
    {
        return { no_value_t{}};
    }

    /**
     * This serializer_t specialization serializes the "WHEN <expr> THEN <expr> part of
     * a CASE expression.
     */
    template< typename Context, typename WhenExpr, typename ThenExpr>
    struct serializer_t< Context, when_t< WhenExpr, ThenExpr>>
    {
        using _serialize_check = serialize_check_of<Context, WhenExpr, ThenExpr>;
        using T = when_t< WhenExpr, ThenExpr>;

        static Context & _( const T& t, Context &context)
        {
            context << "WHEN ";
            serialize_operand( t._when, context);
            context << " THEN ";
            serialize_operand( t._then, context);
            return context;
        }
    };

    /**
     * This specialization serializes a CASE-expression.
     */
    template< typename Context, typename CaseExpr, typename ElseExpr, typename WhenTuple>
    struct serializer_t< Context, case_t<CaseExpr, ElseExpr, WhenTuple>>
    {
        using T = case_t<CaseExpr, ElseExpr, WhenTuple>;
        using _serialize_check = detail::serialize_check_vector_t<Context, typename T::_nodes>;

        int x = _serialize_check{};
        static void serialize_if_value( const char *, no_value_t, Context &)
        {
            // do nothing/intentionally left blank.
        }

        template <typename T>
        static void serialize_if_value( const char *prefix, const T &t, Context &context)
        {
            context << prefix;
            serialize_operand( t, context);
        }

        static Context & _( const T& t, Context &context)
        {
            context << "CASE ";
            serialize_if_value( "", t._case, context);
            context << ' ';
            interpret_tuple_without_braces( t._when, ' ', context);
            serialize_if_value( " ELSE ", t._else, context);
            context << " END";
            return context;
        }
    };

}




#endif /* _SQLPP_CASE_H_ */
