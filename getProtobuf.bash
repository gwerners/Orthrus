#!/bin/bash
VERSION=3.5.1
curl -OL https://github.com/google/protobuf/releases/download/v${VERSION}/protoc-${VERSION}-linux-x86_64.zip
unzip protoc-${VERSION}-linux-x86_64.zip -d protoc3
sudo mv protoc3/bin/* /usr/local/bin/
sudo mv protoc3/include/* /usr/local/include/
rm protoc-${VERSION}-linux-x86_64.zip
rm -rf protoc3
