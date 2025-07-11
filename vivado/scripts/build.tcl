open_project ../hornet_fhe.xpr

reset_run synth_1
launch_runs synth_1 -jobs 12
wait_on_run synth_1

launch_runs impl_1 -jobs 12 
wait_on_run impl_1

launch_runs impl_1 -to_step write_bitstream -jobs 12
wait_on_run impl_1 