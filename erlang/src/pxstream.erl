-module(pxstream).

-export([main/0, get_position/2, base_4_digs/1, pxs_displayer/3, do/0]).

% Helper Functions

base_4_digs(0) -> 0;
base_4_digs(Num) ->
	1 + base_4_digs(Num bsr 2).


virtual_width(X, Y) when X < Y -> virtual_width(Y, X);
virtual_width(Width, _) ->
	floor(math:pow(2, ceil(math:log2(Width)))).


% get_position({1, 36, 36}, {0, 0, 0, 0, 64})

%						{consts}						{vars}
get_position({Index, _W, _H}, {X, Y, Inc, ShiftCount, _CurrQuadWidth})
  when ((Index+Inc) bsr (ShiftCount*2)) == 0 -> % 
	{floor(Index+Inc), floor(X), floor(Y)};
get_position({Index, W, H}, {X, Y, Inc, _ShiftCount, _CurrQuadWidth})
  when X > W; Y > H ->
	get_position({Index, W, H}, {0, 0, Inc+1, 0, virtual_width(W, H)});
get_position({Index, W, H}, {X, Y, Inc, ShiftCount, CurrQuadWidth}) -> %QuadWidth from virtual_width(), X and Y are the position so far, starting at 0
	NewIdx = Index + Inc,
	CoordInc = CurrQuadWidth / 2,
	case (NewIdx bsr (ShiftCount*2)) rem 4 of
		0 -> get_position({Index, W, H}, {X,          Y,          Inc, ShiftCount+1, CurrQuadWidth / 2});
		3 -> get_position({Index, W, H}, {X+CoordInc, Y,          Inc, ShiftCount+1, CurrQuadWidth / 2});
		2 -> get_position({Index, W, H}, {X,          Y+CoordInc, Inc, ShiftCount+1, CurrQuadWidth / 2});
		1 -> get_position({Index, W, H}, {X+CoordInc, Y+CoordInc, Inc, ShiftCount+1, CurrQuadWidth / 2})
	end.




% Display server

start_pxs_display(Width, Height)->
	Im = egd:create(Width, Height),
%	egd:filledRectangle(Im, {3, 3}, {2, 2}, {255, 0, 0}),
	spawn(?MODULE, pxs_displayer, [{Width, Height}, Im, 0]).


pxs_display(Pid, Pixel)-> % Pixel is {R, G, B}.
	Pid ! {self(), Pixel},
	receive
		Response -> Response
	end.


pxs_displayer({Width, Height}, Im, Index)->
	receive
		{From, done} ->
			egd:save(egd:render(Im, png), "test1.png"),
			egd:destroy(Im),
			From ! finished,
			pxs_displayer({Width, Height}, Im, Index);
		{From, Pixel} ->
			{NewIdx, X, Y} = get_position({Index, Width, Height}, {0, 0, 0, 0, virtual_width(Width, Height)}),
			From ! ok,
			draw_pixel(Im, X, Y, Pixel),
			pxs_displayer({Width, Height}, Im, NewIdx+1)

	end.


draw_pixel(Im, X, Y, Color) ->
	egd:filledRectangle(Im, {X, Y}, {X, Y}, egd:color(Color)).



% Controller code

main()->
	% {ok, InputFile} = io:read("Enter the file: "),
	% io:format("Args: ~p\n", [InputFile]),
	{ok, Data} = file:read_file("out-B24.pxs"),
	pxstream_process(binary_to_list(Data)).


pxstream(Pid, []) ->
	pxs_display(Pid, done);
pxstream(Pid, Data)->
	[R, G, B | Rest] = Data,
	pxs_display(Pid, {R, G, B}),
	pxstream(Pid, Rest).


pxstream_process(File)->
	[X1, X2, X3, X4, Y1, Y2, Y3, Y4 | Rest ] = File,
	Width = X1 bsl 24 + X2 bsl 16 + X3 bsl 8 + X4, % Image Width, 32-bit unsigned int
	Height = Y1 bsl 24 + Y2 bsl 16 + Y3 bsl 8 + Y4, % Image Height, 32-bit unsigned int
	Pid = start_pxs_display(Width, Height),
	pxstream(Pid, Rest).


do() ->
    Im = egd:create(200,200),
    Red = egd:color({255,0,0}),
    Black = egd:color({0,0,0}),

    % Line and fillRectangle

    egd:filledRectangle(Im, {20,20}, {180,180}, Red),
    egd:line(Im, {0,0}, {200,200}, Black),    

    egd:save(egd:render(Im, png), "test1.png"),

    egd:destroy(Im).




-ifdef(EUNIT).

-include_lib("eunit/include/eunit.hrl").


base_4_digs_test_() -> [
	?_assertEqual(2, base_4_digs(15)),
	?_assertEqual(3, base_4_digs(16))
].

virtual_width_test_() -> [
	?_assertEqual(16, virtual_width(4, 9))
].

% get_position({Index, W, H}, {X, Y, Inc, ShiftCount, CurrQuadWidth})
get_position_test_() -> [
	?_assertEqual({0, 0, 0}, get_position({0, 36, 36}, {0, 0, 0, 0, 64})),
	?_assertEqual({1, 32, 32}, get_position({1, 36, 36}, {0, 0, 0, 0, 64})),
	?_assertEqual({1, 0, 32}, get_position({1, 12, 36}, {0, 0, 0, 0, 64}))
].

-endif.




