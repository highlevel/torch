require "torch"
cutorch = require "libcutorch"

torch.CudaStorage.__tostring__ = torch.FloatStorage.__tostring__
torch.CudaTensor.__tostring__ = torch.FloatTensor.__tostring__

torch.include('cutorch', 'Tensor.lua')

return cutorch
