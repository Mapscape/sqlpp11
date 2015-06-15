/*
 * create_table.h
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

#ifndef SQLPP11_CREATE_TABLE_H_
#define SQLPP11_CREATE_TABLE_H_
#include <tuple>
#include <type_traits>
#include <sqlpp11/table.h>
#include <sqlpp11/statement.h>
#include <sqlpp11/select_column_list.h>
#include <sqlpp11/interpret_tuple.h>
#include <sqlpp11/detail/index_sequence.h>
#include <sqlpp11/detail/get_first.h>

namespace sqlpp
{

    /// Template meta function that tells us whether type T is
    /// some instantiation of the select_column_list_t<> template.
    template< typename T>
    struct is_select_column_list
            : std::false_type {};

    template< typename... Args>
    struct is_select_column_list<select_column_list_t<Args...>>
            : std::true_type {};



    /// Given either a table or a (select-) statement, obtain a tuple
    /// type that holds all columns.
    /// The default implementation assumes a table type is given and
    /// extracts the embedded _column_tuple_t.
    template< typename TableType>
    struct get_column_tuple_t
    {
        using type = typename TableType::_column_tuple_t;
    };


    template <typename... StatementPolicies>
    struct get_column_tuple_t<
        statement_t<StatementPolicies...>
        >
    {
        using column_list_t = detail::get_first_if< is_select_column_list, void, StatementPolicies...>;
        using type = typename get_column_tuple_t< column_list_t>::type;
    };

    template< typename Database, typename... Columns>
    struct get_column_tuple_t<
        select_column_list_t< Database, Columns...>
        >
    {
        using type = std::tuple< Columns...>;
    };

    template <typename TableType, typename Enable = void>
    struct get_primary_key_columns
    {
        using type = detail::type_set<>;
    };

    template< typename TableType>
    struct get_primary_key_columns<TableType, typename std::enable_if<std::is_class<typename TableType::_primary_key_columns>::value, void>::type>
    {
        using type = typename TableType::_primary_key_columns;
    };

    // type that represents a create table statement.
    // all information about the table itself is containted in the TableOrStatement.
    template< typename TableOrStatement, typename AliasType = void>
    struct create_table_t
    {
        using _name_t               = typename std::conditional<
                                            std::is_same< void, AliasType>::value,
                                            TableOrStatement,
                                            AliasType>::type;
        using _alias_t              = typename _name_t::_alias_t;
        using _traits               = make_traits<no_value_t, tag::is_statement>;
        using _run_check            = consistent_t;
        using _table_type           = TableOrStatement;
        using _column_tuple_t       = typename get_column_tuple_t<TableOrStatement>::type;
        using _primary_key_columns  = typename get_primary_key_columns<TableOrStatement>::type;
        template< typename Database>
        void _run( Database &db ) const
        {
            db.create( *this);
        }
    };

    // wrapper type to be able to properly specialize a serializer for the
    // create-table text for a column
    template <typename ColumnDef>
    struct create_column
    {

    };

    // wrapper type to make sure that a column def gets serialized as an
    // unqualified name instead of a table.columnname-combination, which is the default
    // for serialization of columndefs.
    template <typename ColumnDef>
    struct simple_column_name
    {
    };


    // serialize a simple column name (without its table name).
    template<typename Context, typename ColumnDef>
    struct serializer_t< Context, simple_column_name< ColumnDef>>
    {
        using T = simple_column_name< ColumnDef>;
        static Context& _(const T& , Context& context)
        {
            context << name_of<ColumnDef>::char_ptr();
            return context;
        }
    };

    // creates the text for a single column in a create table statement.
    template<typename Context, typename ColumnDef>
    struct serializer_t< Context, create_column< ColumnDef>>
    {
        using T = create_column< ColumnDef>;
        struct column_options_writer
        {
            column_options_writer( Context &context)
            :_context( context)
            {

            }


            template< typename T>
            T &represent( T &val)
            {
                return val;
            }

            std::string represent( const std::string &val)
            {
                return '\'' + _context.escape( val) + '\'';
            }

            std::string represent( const char *val)
            {
                return std::string("'") + _context.escape( val) + "'";
            }

            std::string represent( const null_t &)
            {
                return "NULL";
            }

            template< typename ValueType>
            void operator()( const tag::has_default<ValueType> &def)
            {
                _context << " DEFAULT " << represent( def.value);
            }

            void operator()( const tag::is_auto_increment &)
            {
                _context << " AUTOINCREMENT";
            }

            template< typename OtherType>
            void operator()( const OtherType &)
            {
            }

            template< typename Tuple, size_t... Indexes>
            void operator()( const Tuple &options, const detail::index_sequence<Indexes...>&)
            {
                using swallow = int[];
                (void)swallow{
                    0, ((*this)(std::get<Indexes>(options)),0)...
                };
            }

            template< typename... Options>
            void operator()( std::tuple<Options...> options)
            {
                (*this)( options, detail::make_index_sequence<sizeof...(Options)>{});
            }
        private:
           Context &_context;
        };

        // output the create text for a single column.
        static Context& _(const T& , Context& context)
        {
            context << name_of<ColumnDef>::char_ptr();
            context << " ";
            context << value_type_of<ColumnDef>::_name_t::char_ptr();

            column_options_writer writer( context);
            writer( ColumnDef::_spec_t::traits_values);

            return context;
        }
    };

    // construct a CREATE TABLE statement for the given table or statement.
    template<typename Context, typename TableOrStatement, typename AliasProvider>
        struct serializer_t<Context, create_table_t<TableOrStatement, AliasProvider>>
        {

            using _serialize_check = consistent_t;

            using T = create_table_t<TableOrStatement, AliasProvider>;

            template< typename... ColumnDefs>
            static void serialize_columns( const std::tuple<ColumnDefs...> &, Context &context)
            {
                interpret_tuple( std::tuple<create_column<ColumnDefs>...>{}, ",\n", context);
            }

            // declare the given columns as keys.
            template<typename... KeyColumns>
            static void serialize_keys( const detail::type_set< KeyColumns...> &, Context &context)
            {
                context << "\n,PRIMARY KEY(";
                interpret_tuple( std::tuple<simple_column_name<KeyColumns>...>{}, ',', context);
                context << ")";
            }

            static void serialize_keys( const detail::type_set<> &, Context &)
            {
                // intentionally left blank. We don't emit anything for an empty set of keys
            }

            // declare the keys of a table type.
            static void serialize_keys( const T& t, Context& context)
            {
                serialize_keys( typename T::_primary_key_columns{}, context);
            }

            static Context& _(const T& , Context& context)
            {
                context << "CREATE TABLE " << name_of<T>::char_ptr() << "(\n";
                serialize_columns( typename T::_column_tuple_t{},       context);
                serialize_keys(    typename T::_primary_key_columns{},  context);
                context << ')';

                return context;
            }
        };

    template< typename Table>
    create_table_t<Table> create_table( const Table &)
    {
        return {};
    }

    template< typename TableOrStatement, typename AliasProvider>
    create_table_t<TableOrStatement, AliasProvider> create_table( const TableOrStatement &, const AliasProvider &)
    {
        return {};
    }

}

#endif /* SQLPP11_CREATE_TABLE_H_ */
