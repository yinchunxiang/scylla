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
 *
 * Modified by Cloudius Systems.
 * Copyright 2015 Cloudius Systems.
 */

#pragma once

#include "utils/UUID.hh"
#include "gms/inet_address.hh"
#include "streaming/stream_session.hh"
#include "streaming/session_info.hh"
#include "streaming/progress_info.hh"

namespace streaming {

class stream_event {
public:
    using UUID = utils::UUID;
    enum class type {
        STREAM_PREPARED,
        STREAM_COMPLETE,
        FILE_PROGRESS,
    };

    type event_type;
    UUID plan_id;

    stream_event(type event_type_, UUID plan_id_)
        : event_type(event_type_)
        , plan_id(plan_id_) {
    }
};

struct session_complete_event : public stream_event {
    using inet_address = gms::inet_address;
    inet_address peer;
    bool success;
    int session_index;

    session_complete_event(stream_session& session)
        : stream_event(stream_event::type::STREAM_COMPLETE, session.plan_id())
        , peer(session.peer)
        , success(session.is_success())
        , session_index(session.session_index()) {
    }
};

struct progress_event : public stream_event {
    using UUID = utils::UUID;
    progress_info progress;
    progress_event(UUID plan_id_, progress_info progress_)
        : stream_event(stream_event::type::FILE_PROGRESS, plan_id_)
        , progress(std::move(progress_)) {
    }
#if 0
    @Override
    public String toString()
    {
        return "<ProgressEvent " + progress + ">";
    }
#endif
};

struct session_prepared_event : public stream_event {
    using UUID = utils::UUID;
    session_info session;
    session_prepared_event(UUID plan_id_, session_info session_)
        : stream_event(stream_event::type::STREAM_PREPARED, plan_id_)
        , session(std::move(session_)) {
    }
};

} // namespace streaming