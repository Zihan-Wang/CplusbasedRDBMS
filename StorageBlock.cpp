//
//  StorageBlock.cpp
//  Assignment3
//
//  Created by rick gessner on 4/11/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "StorageBlock.hpp"

namespace ECE141 {

  StorageBlock::StorageBlock(BlockType aType) : header(aType) {
    memset(data,' ', sizeof(data)-1);
    data[sizeof(data) - 1] = 0;
  }

  StorageBlock::StorageBlock(const StorageBlock &aCopy) : header(aCopy.header) {
    std::memcpy(reinterpret_cast<void*>(data), &aCopy.data, kPayloadSize);
  }

  StorageBlock& StorageBlock::operator=(const StorageBlock &aCopy) {
    header=aCopy.header;
    std::memcpy(reinterpret_cast<void*>(data), &aCopy.data, kPayloadSize);
    return *this;
  }
  
  //USE: save the given stream in this block data area.
  StorageBlock& StorageBlock::store(std::iostream &aStream) {
      aStream >> data;
    return *this;
  }
}
