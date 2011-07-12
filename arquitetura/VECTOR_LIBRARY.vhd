LIBRARY ieee;
USE ieee.std_logic_1164.all;

PACKAGE VECTOR_LIBRARY IS

	TYPE motion_vector IS ARRAY (0 TO 1) OF STD_LOGIC_VECTOR(7 DOWNTO 0);
	TYPE state IS (inicio, idle, set_mux, bypass, loadA, loadB, loadC, carga_reg_ABC, carga_reg_A, carga_reg_BC, computa0, computa1, carga_regs, refresh1, refresh2, refresh3_0, refresh3_1);

	COMPONENT mediana IS
		PORT (
			a, b, c : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			s : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
	END COMPONENT;


END VECTOR_LIBRARY;
