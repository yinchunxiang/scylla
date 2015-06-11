/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
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
 * Copyright 2015 Cloudius Systems
 *
 * Modified by Cloudius Systems
 */

#include "cql3/statements/create_table_statement.hh"

#include "schema_builder.hh"

namespace cql3 {

namespace statements {

create_table_statement::create_table_statement(::shared_ptr<cf_name> name,
                                               ::shared_ptr<cf_prop_defs> properties,
                                               bool if_not_exists,
                                               std::set<::shared_ptr<column_identifier>> static_columns)
    : schema_altering_statement{name}
    , _static_columns{static_columns}
    , _properties{properties}
    , _if_not_exists{if_not_exists}
{
#if 0
    try
    {
        if (!this.properties.hasProperty(CFPropDefs.KW_COMPRESSION) && CFMetaData.DEFAULT_COMPRESSOR != null)
            this.properties.addProperty(CFPropDefs.KW_COMPRESSION,
                                        new HashMap<String, String>()
                                        {{
                                            put(CompressionParameters.SSTABLE_COMPRESSION, CFMetaData.DEFAULT_COMPRESSOR);
                                        }});
    }
    catch (SyntaxException e)
    {
        throw new AssertionError(e);
    }
#endif
}

void create_table_statement::check_access(const service::client_state& state) {
    warn(unimplemented::cause::PERMISSIONS);
#if 0
    state.hasKeyspaceAccess(keyspace(), Permission.CREATE);
#endif
}

void create_table_statement::validate(service::storage_proxy&, const service::client_state& state) {
    // validated in announceMigration()
}

// Column definitions
std::vector<column_definition> create_table_statement::get_columns()
{
    std::vector<column_definition> column_defs;
    for (auto&& col : _columns) {
        column_kind kind = column_kind::regular_column;
        if (_static_columns.count(col.first)) {
            kind = column_kind::static_column;
        }
        column_defs.emplace_back(col.first->name(), col.second, kind);
    }
    return column_defs;
}

future<bool> create_table_statement::announce_migration(service::storage_proxy& proxy, bool is_local_only) {
    return service::migration_manager::announce_new_column_family(proxy, get_cf_meta_data(), is_local_only).then_wrapped([this] (auto&& f) {
        try {
            f.get();
            return true;
        } catch (const exceptions::already_exists_exception& e) {
            if (_if_not_exists) {
                return false;
            }
            throw e;
        }
    });
}

shared_ptr<transport::event::schema_change> create_table_statement::change_event() {
    return make_shared<transport::event::schema_change>(transport::event::schema_change::change_type::CREATED, transport::event::schema_change::target_type::TABLE, keyspace(), column_family());
}

/**
 * Returns a CFMetaData instance based on the parameters parsed from this
 * <code>CREATE</code> statement, or defaults where applicable.
 *
 * @return a CFMetaData instance corresponding to the values parsed from this statement
 * @throws InvalidRequestException on failure to validate parsed parameters
 */
schema_ptr create_table_statement::get_cf_meta_data() {
    schema_builder builder{keyspace(), column_family()};
    apply_properties_to(builder);
    return builder.build();
}

void create_table_statement::apply_properties_to(schema_builder& builder) {
    auto&& columns = get_columns();
    for (auto&& column : columns) {
        builder.with_column(column);
    }
#if 0
    cfmd.defaultValidator(defaultValidator)
        .keyValidator(keyValidator)
        .addAllColumnDefinitions(getColumns(cfmd))
        .isDense(isDense);
#endif

    add_column_metadata_from_aliases(builder, _key_aliases, _key_validator, column_kind::partition_key);
#if 0
    addColumnMetadataFromAliases(cfmd, columnAliases, comparator.asAbstractType(), ColumnDefinition.Kind.CLUSTERING_COLUMN);
    if (valueAlias != null)
        addColumnMetadataFromAliases(cfmd, Collections.singletonList(valueAlias), defaultValidator, ColumnDefinition.Kind.COMPACT_VALUE);
#endif

    _properties->apply_to_builder(builder);
}

void create_table_statement::add_column_metadata_from_aliases(schema_builder& builder, std::vector<bytes> aliases, data_type comparator, column_kind kind)
{
#if 0
    if (comparator instanceof CompositeType)
    {
        CompositeType ct = (CompositeType)comparator;
        for (int i = 0; i < aliases.size(); ++i)
            if (aliases.get(i) != null)
                cfm.addOrReplaceColumnDefinition(new ColumnDefinition(cfm, aliases.get(i), ct.types.get(i), i, kind));
    }
    else
    {
#endif
        assert(aliases.size() <= 1);
        if (!aliases.empty())
            builder.with_column(aliases[0], comparator, kind);
#if 0
    }
#endif
}

}

}