LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
LIBRARY WORK;
USE WORK.VECTOR_LIBRARY.ALL;

ENTITY control IS
	PORT (
			clk, reset, start: IN STD_LOGIC;
			block_type: IN STD_LOGIC_VECTOR(1 DOWNTO 0);
			we: OUT STD_LOGIC;
			enable_reg_A, enable_reg_B, enable_reg_C: OUT STD_LOGIC;
			enable_reg0, enable_reg1, enable_reg2, enable_reg3: OUT STD_LOGIC;
			mux_A, mux_B, mux_C, mux_write_memory: OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
			memory_address: OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
END control;

ARCHITECTURE comportamento OF control IS

SIGNAL px, py, p_mem: STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL estado, proximo: state;
SIGNAL sinal: STD_LOGIC;

BEGIN
	
	PROCESS(clk,reset)
	BEGIN
		IF(reset = '1') THEN
			estado <= inicio;
			sinal <= '0';
		ELSIF(clk'EVENT AND clk = '1') THEN
			estado <= proximo;
			sinal <= NOT sinal;
		END IF;
	END PROCESS;

	PROCESS(sinal, estado)
	BEGIN
		CASE estado IS
		WHEN inicio =>
			px <= "00000010";
			py <= "00000010";
			we <= '0';
			enable_reg_A <= '0';
			enable_reg_B <= '0';
			enable_reg_C <= '0';
			enable_reg0 <= '0';
			enable_reg1 <= '0';
			enable_reg2 <= '0';
			enable_reg3 <= '0';
			p_mem <= "00000011";
			memory_address <= (OTHERS=>'0');
			proximo <= idle;
			
		WHEN idle =>
			IF start = '1' THEN
				proximo <= set_mux;
			ELSE
				proximo <= idle;
			END IF;
			we <= '0';
			enable_reg_A <= '0';
			enable_reg_B <= '0';
			enable_reg_C <= '0';
			enable_reg0 <= '0';
			enable_reg1 <= '0';
			enable_reg2 <= '0';
			enable_reg3 <= '0';
			
		WHEN set_mux =>
			CASE block_type IS
			WHEN "00" =>
				IF px = "00000000" THEN
					mux_A <= "11";
					mux_B <= "00";
					mux_C <= "00";
				ELSE
					IF py = "00000000" THEN
						mux_A <= "00";
						mux_B <= "11";
						mux_C <= "11";
					ELSE
						mux_A <= "00";
						mux_B <= "00";
						mux_C <= "00";
					END IF;
				END IF;
			WHEN "01" =>
				IF py = "00000000" THEN
					mux_A <= "01";
					mux_B <= "11";
					mux_C <= "11";
				ELSE
					mux_A <= "01";
					mux_B <= "00";
					mux_C <= "00";
				END IF;
			WHEN "10" =>
				IF px = "00000000" THEN
					mux_A <= "11";
					mux_B <= "01";
					mux_C <= "01";
				ELSE	
					mux_A <= "00";
					mux_B <= "01";
					mux_C <= "01";
				END IF;
			WHEN OTHERS => --CASE "11"
				mux_A <= "10";
				mux_B <= "10";
				mux_C <= "10";
			END CASE;

			IF (px = "00000000" AND py = "00000000") THEN
				proximo <= bypass;
			ELSIF (px = "00000000" AND block_type = "10") OR (py = "00000000" AND block_type = "01") OR (block_type = "11") THEN
				proximo <= carga_reg_ABC;
			ELSIF (px = "00000000" AND block_type = "00") OR (block_type = "01") THEN
        proximo <= carga_reg_A;
			ELSE --(px != "00000000" AND (block_type = "00")) THEN
				proximo <= loadA;
			END IF;
		WHEN bypass =>
			--ENABLE MUX BYPASS
			px <= px + "00000001";
			proximo <= idle;
			
		WHEN loadA =>
			enable_reg_A <= '1';
			IF (block_type = "00" AND py = "00000000") OR (block_type = "10") THEN
				proximo <= carga_reg_BC;
			ELSE
				proximo <= loadB;
			END IF;
			
			IF block_type = "00" THEN
			  memory_address <= "11110001";
			ELSE
			  memory_address <= "11110010";
			END IF;
			
		WHEN loadB =>
			enable_reg_B <= '1';
			proximo <= loadC;
			memory_address <= p_mem;
			
		WHEN loadC =>
			enable_reg_C <= '1';
			proximo <= computa0;
			IF block_type = "00" THEN
			 memory_address <= p_mem + "00000001";
      ELSE 
        memory_address <= p_mem + "00000010";        
      END IF;
			
		WHEN carga_reg_ABC =>
			enable_reg_A <= '1';
			enable_reg_B <= '1';
			enable_reg_C <= '1';
			proximo <= computa0;
			
		WHEN carga_reg_A =>
			enable_reg_A <= '1';
			proximo <= loadB;
			
		WHEN carga_reg_BC =>
			enable_reg_B <= '1';
			enable_reg_C <= '1';
			proximo <= computa0;
		WHEN computa0 =>
			proximo <= computa1;
		WHEN computa1 =>
		  proximo <= carga_regs;	
			
		WHEN carga_regs =>
		  CASE block_type IS
      WHEN "00" =>
        enable_reg0 <= '1';
      WHEN "01" =>
        enable_reg1 <= '1';
      WHEN "10" =>
        enable_reg2 <= '1';
      WHEN OTHERS =>
        enable_reg3 <= '1';
		  END CASE;		  
		  IF block_type = "11" THEN
		    proximo <= refresh1;
		  ELSE
		    proximo <= idle;
		  END IF;
			
		WHEN refresh1 =>
			mux_write_memory <= "00";
			memory_address <= "11110001"; --241
			we <= '1';
			proximo <= refresh2;
		WHEN refresh2 =>
			mux_write_memory <= "01";
			memory_address <= p_mem - "00000001";
			we <= '1';
			proximo <= refresh3_0;
		WHEN refresh3_0 =>
		  mux_write_memory <= "10"; 
			memory_address <= "11110010"; --242
			we <= '1';
			proximo <= refresh3_1;
		WHEN OTHERS =>-- refresh3_1 =>
			mux_write_memory <= "10";
			memory_address <= p_mem;
			we <= '1';
			proximo <= idle;
			CASE block_type IS
			  WHEN "00" =>
			    px <= px + "00000001";
			  WHEN "01" =>
			    px <= px - "00000001";
			    py <= py + "00000001";
			  WHEN "10" =>
			    px <= px + "00000001";
			  WHEN OTHERS =>
			    px <= px + "00000001";
			    py <= py - "00000001";
			END CASE;
			IF block_type = "11" THEN
			  p_mem <= p_mem + "00000010";
			END IF;
		END CASE;
	END PROCESS;

END comportamento;
