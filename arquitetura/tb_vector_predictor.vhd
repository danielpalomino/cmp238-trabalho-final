
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;
USE ieee.numeric_std.ALL;

LIBRARY work;
USE work.VECTOR_LIBRARY.ALL;
 
ENTITY tb_vector_predictor IS
END tb_vector_predictor;
 
ARCHITECTURE behavior OF tb_vector_predictor IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
	COMPONENT vector_predictor
		PORT(
			clk : IN  std_logic;
			reset : IN  std_logic;
			start : IN  std_logic;
			block_type : IN  std_logic_vector(1 downto 0);
			mv_in : IN  motion_vector;
			done : OUT STD_LOGIC;
			mv_out : OUT  motion_vector
		);
END COMPONENT;

	COMPONENT file_read IS
		GENERIC (stim_file: string := "sim.dat");
		PORT (
			start : IN  std_logic;
			block_type : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
			mv : OUT motion_vector;
			EOF : OUT std_logic
		);
	END COMPONENT;
	
   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal start : std_logic := '0';
   signal block_type : std_logic_vector(1 downto 0) := (others => '0');
   signal mv_in : motion_vector;
   

 	--Outputs
   signal mv_out : motion_vector;
   signal done : STD_LOGIC;
	
  	signal EOF : std_logic;

   -- Clock period definitions
   constant clk_period : time := 20ns;
	
 
BEGIN

	FREAD_0: file_read PORT MAP (
		start => start,
		block_type => block_type,
		mv => mv_in,
		EOF => EOF
	);
 
	-- Instantiate the Unit Under Test (UUT)
   uut: vector_predictor PORT MAP (
          clk => clk,
          reset => reset,
          start => start,
          block_type => block_type,
          mv_in => mv_in,
          mv_out => mv_out,
          done => done
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 

   -- Stimulus process
   stim_proc: process
   begin		
		reset <= '1';
      wait for clk_period;
		reset <='0';	
		
		while EOF = '0' LOOP
			start <= '1';
			wait for clk_period;
			start <= '0';
			wait for clk_period * 10;
      end loop;
		
		wait;
		
   end process;

END behavior;
