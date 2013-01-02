import unittest as unittest
import numpy as np
import rftk.assert_util
import rftk.buffers as buffers
import rftk.features as features
import rftk.utils.buffer_converters as buffer_converters

class TestAxisAlignedFeature(unittest.TestCase):

    def test_axis_aligned_feature_extractor(self):
        data = buffer_converters.as_matrix_buffer(np.array([[2.0,21,1],[3.0,22,5]], dtype=np.float32))
        axis_aligned_feature_extractor = features.AxisAlignedFeatureExtractor(data)

        sample_indices = buffer_converters.as_matrix_buffer(np.array(range(2), dtype=np.int32))
        int_feature_params = buffer_converters.as_matrix_buffer(np.array([1,0,2,0], dtype=np.int32))
        float_feature_params = buffer_converters.as_matrix_buffer(np.array([0,0,0,0], dtype=np.float32))
        results_buffer = buffers.MatrixBufferFloat()
        axis_aligned_feature_extractor.Extract(sample_indices, 
                                                int_feature_params, 
                                                float_feature_params, 
                                                results_buffer)
        results = buffer_converters.as_numpy_array(results_buffer)
        expected_results = np.array([[21,22],[2,3],[1,5],[2,3]], dtype=np.float32)
        
        self.assertTrue((results == expected_results).all())



if __name__ == '__main__':
    unittest.main()