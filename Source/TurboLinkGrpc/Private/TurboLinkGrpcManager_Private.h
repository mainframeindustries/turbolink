#pragma once

#include "TurboLinkGrpcManager.h"

#include <grpcpp/grpcpp.h>
#include <set>

class JWTTokenAuthenticator : public grpc::MetadataCredentialsPlugin {
 public:
  JWTTokenAuthenticator(const grpc::string& ticket) : ticket_(ticket) {}

  virtual grpc::Status GetMetadata(
      grpc::string_ref service_url, grpc::string_ref method_name,
      const grpc::AuthContext& channel_auth_context,
      std::multimap<grpc::string, grpc::string>* metadata) override {
    metadata->insert(std::make_pair("x-custom-auth-ticket", ticket_));
    return grpc::Status::OK;
  }

 private:
  grpc::string ticket_;
};

class UTurboLinkGrpcManager::Private
{
public:
	struct ServiceChannel
	{
		std::shared_ptr<grpc::Channel> RpcChannel;
		grpc_connectivity_state ChannelState;
		std::string EndPoint;
		std::set<UGrpcService*> AttachedServices;

		bool UpdateState()
		{
			grpc_connectivity_state newState = RpcChannel->GetState(false);
			if (newState == ChannelState) return false;

			ChannelState = newState;
			return true;
		}
	};

public:
	static EGrpcServiceState GrpcStateToServiceState(grpc_connectivity_state State);

	std::shared_ptr<ServiceChannel> CreateServiceChannel(const char* EndPoint, UGrpcService* AttachedService);
	void RemoveServiceChannel(std::shared_ptr<ServiceChannel> Channel, UGrpcService* AttachedService);

	static std::unique_ptr<grpc::ClientContext> CreateRpcClientContext();

	void ShutdownCompletionQueue();

public:
	std::map<std::string, std::shared_ptr<ServiceChannel>> ChannelMap;
	std::unique_ptr<grpc::CompletionQueue> CompletionQueue;
	FString AuthToken = "";
};
