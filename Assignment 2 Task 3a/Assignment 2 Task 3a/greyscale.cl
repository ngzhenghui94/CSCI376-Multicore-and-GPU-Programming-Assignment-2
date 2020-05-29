__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | 
      CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST; 

__kernel void greyscale(read_only image2d_t src_image,
						write_only image2d_t dst_image) {

   /* Get pixel coordinate */
   int2 coord = (int2)(get_global_id(0), get_global_id(1));

   /* Read pixel value */
   float4 pixel = read_imagef(src_image, sampler, coord);

   /* Luminance Value */
   float luminance = sqrt(0.299f * pixel.x * pixel.x + 0.587f * pixel.y * pixel.y + 0.114f * pixel.z * pixel.z);

   /* Greyscale each pixel based */
   pixel.xyz = luminance;

   /* Write new pixel value to output */
   write_imagef(dst_image, coord, pixel);
}
