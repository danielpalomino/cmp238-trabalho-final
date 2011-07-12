LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
USE PACKAGE VECTOR_LIBRARY;

ENTITY control IS
	PORT (
			clk, reset, start: IN STD_LOGIC;
			block_type: IN STD_LOGIC_VECTOR(1 DOWNTO 0);
			we: OUT STD_LOGIC;
			enable_reg_A, enable_reg_B, enable_reg_C: OUT STD_LOGIC;
			enable_reg0, enable_reg1, enable_reg2, enable_reg3: OUT STD_LOGIC;
			memory_address: OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
END control;

ARCHITECTURE comportamento OF control IS

SIGNAL px, py: STD_LOGIC_VECTOR(7 DOWNTO 0);
SIGNAL estado, proximo: state;

BEGIN
	
	PROCESS(clk,reset)
	BEGIN
		IF(reset = '1') THEN
			start <= '0';
			estado <= idle;
			px <= (OTHERS=>'0');
			py <= (OTHERS=>'0');
			we <= '0';
			enable_reg_A <= '0';
			enable_reg_B <= '0';
			enable_reg_C <= '0';			
			memory_address <= (OTHERS=>'0');
		ELSIF(clk'EVENT AND clk = '1') THEN
			estado <= proximo;
		END IF;
	END PROCESS;

	PROCESS(estado)
	BEGIN
		CASE estado IS
		WHEN idle =>
			IF start = '1' THEN
				proximo <= set_mux;
			ELSE
				proximo <= idle;
			--RESETAR TUDO DENOVO?
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
			WHEN OTHERS => --CASE "11"
				mux_A <= "10";
				mux_B <= "10";
				mux_C <= "10";
			END CASE;

			IF (px = "00000000" AND py = "00000000") THEN
				proximo <= bypass;
			ELSIF (px = "00000000" AND block_type = "00") OR (block_type = "01") THEN
				proximo <= carga_reg_A;
			ELSIF (px = "00000000" AND block_type = "10") OR (py = "00000000" AND block_type = "01") OR (block_type = "11") THEN
				proximo <= carga_reg_ABC;
			ELSE --(px != "00000000" AND (block_type = "00")) THEN
				proximo <= loadA;
			END IF;
		WHEN bypass =>
			--ENABLE MUX BYPASS
			proximo <= idle;
		WHEN loadA =>
			enable_reg_A <= '1';
			IF (block_type = "00" AND py = "00000000") OR (block_type = "10") THEN
				proximo <= carga_reg_BC;
			ELSE
				proximo <= loadB;
			END IF;
		WHEN loadB =>
			enable_regB = '1';
			proximo <= loadC;
		WHEN loadC =>
			enable_reg_C <= '1';
			proximo <= computa0;
		WHEN carga_reg_ABC =>
			enable_reg_A <= '1';
			enable_reg_B <= '1';
			enable_reg_C <= '1';
			proximo <= computa0;
		WHEN carga_reg_A =>
			enable_reg_A <= '1';
			proximo <= computa0;
		WHEN carga_re_BC =>
			enable_reg_B <= '1';
			enable_reg_C <= '1';
			proximo <= computa0;
		WHEN computa0 =>
			proximo <= computa1;
		WHEN computa1 =>
			CASE block_type IS
			WHEN "00" =>
				proximo <= carga_reg0;
			WHEN "01" =>
				proximo <= carga_reg1;
			WHEN "10" =>
				proximo <= carga_reg2;
			WHEN OTHERS =>
				proximo <= carga_reg3;
		WHEN carga_reg0 =>
			enable_reg0 <= '1';
			proximo <= refresh1;
		WHEN carga_reg1 =>
			enable_reg1 <= '1';
			proximo <= refresh1;
		WHEN carga_reg2 =>
			enable_reg2 <= '1';
			proximo <= refresh1;
		WHEN carga_reg3 =>
			enable_reg3 <= '1';
			proximo <= refresh1;
		WHEN refresh1 =>
			--mux write memory
			memory_address <= "11110001"; --241
			we <= '1';
			proximo <= idle;
		WHEN refresh2 =>
			--mux write memory
			memory_address <= px - "00000001";
			we <= '1';
			proximo <= idle;
		WHEN refresh3_0 =>
			--mux write memory
			memory_address <= "11110010"; --242
			we <= '1';
			proximo <= idle;
		WHEN OTHERS =>-- refresh3_1 =>
			--mux write memory
			memory_address <= px;
			we <= '1';
			proximo <= idle;
		END CASE;
	END PROCESS;

END comportamento;
