#pragma once

// Copyright 2019 Caffeine Inc. All rights reserved.

#include "caffeine.h"

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "absl/types/optional.h"
#include "absl/types/variant.h"

namespace caff {
    // TODO: Get C++17 working and use standard versions
    using absl::optional;
    using absl::variant;
    using absl::get;
    using absl::get_if;

    struct Credentials {
        std::string accessToken;
        std::string refreshToken;
        std::string caid;
        std::string credential;

        std::mutex mutex; // TODO: there's surely a better place for this
    };

    struct IceInfo {
        std::string sdp;
        std::string sdpMid;
        int sdpMLineIndex;
    };

    struct FeedStream {
        optional<std::string> id;
        optional<std::string> sourceId;
        optional<std::string> url;
        optional<std::string> sdpOffer;
        optional<std::string> sdpAnswer;
    };

    struct FeedCapabilities {
        bool video;
        bool audio;
    };

    enum class ContentType {
        Game,
        User
    };

    struct FeedContent {
        std::string id;
        ContentType type;
    };

    struct Feed {
        std::string id;
        optional<std::string> clientId;
        optional<std::string> role;
        optional<std::string> description;
        optional<caff_ConnectionQuality> sourceConnectionQuality;
        optional<double> volume;
        optional<FeedCapabilities> capabilities;
        optional<FeedContent> content;
        optional<FeedStream> stream;
    };

    struct Stage {
        optional<std::string> id;
        optional<std::string> username;
        optional<std::string> title;
        optional<std::string> broadcastId;
        optional<bool> upsertBroadcast;
        optional<bool> live;
        optional<std::map<std::string, Feed>> feeds;
    };

    struct Client {
        std::string id;
        bool headless = true;
        bool constrainedBaseline = false;
    };

    struct StageRequest {
        Client client;
        optional<std::string> cursor;
        optional<Stage> stage;

        StageRequest(std::string username, std::string clientId)
        {
            client.id = std::move(clientId);
            stage = Stage{};
            stage->username = std::move(username);
        }
    };

    struct StageResponse {
        std::string cursor;
        double retryIn;
        optional<Stage> stage;
    };

    struct DisplayMessage {
        std::string title;
        optional<std::string> body;
    };

    struct FailureResponse {
        std::string type;
        optional<std::string> reason;
        optional<DisplayMessage> displayMessage;
    };

    using StageResponseResult = variant<StageResponse, FailureResponse>;

    /*
    struct HeartbeatResponse {
        char * connection_quality;
    };
    void caff_set_string(char ** dest, char const * new_value);

    void caff_free_string(char ** str);

    HeartbeatResponse * caff_heartbeat_stream(char const * stream_url, Credentials * creds);

    void caff_free_heartbeat_response(HeartbeatResponse ** response);

    bool caff_update_broadcast_screenshot(
        char const * broadcastId,
        uint8_t const * screenshot_data,
        size_t screenshot_size,
        Credentials * creds);
    */

    // TODO: not pointers
    caff_GameList * getSupportedGames();
    bool isSupportedVersion();
    caff_AuthResponse * signin(char const * username, char const * password, char const * otp);
    Credentials * refreshAuth(char const * refreshToken);
    caff_UserInfo * getUserInfo(Credentials & creds);

    bool trickleCandidates(
        std::vector<caff::IceInfo> const & candidates,
        std::string const & streamUrl,
        Credentials & credentials);

    bool requestStageUpdate(StageRequest & request, Credentials & creds, double * retryIn, bool * isOutOfCapacity);

}