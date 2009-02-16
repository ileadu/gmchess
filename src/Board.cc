/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * Board.h
 * Copyright (C) wind 2009 <xihels@gmail.com>
 * 
 * Board.h is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Board.h is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Board.h"
#include <vector>
#include "Engine.h"

const int border_width = 32;
const int chessman_width = 57;

#define	PLACE_LEFT 0x01
#define PLACE_RIGHT 0x02
#define PIECE_START 16  //棋子开始数字
#define PIECE_END   48  //棋子结束数字
#define PLACE_ALL PLACE_LEFT | PLACE_RIGHT
#define DATA_DIR "../data/wood/"


Board::Board() :
	selected_x(-1),
	selected_y(-1),
	selected_chessman(-1)
{
	bg_image = Gdk::Pixbuf::create_from_file(DATA_DIR"bg.png");
	this->set_size_request(521,577);
	chessman_images[BLACK_ADVISOR] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_advisor.png");
	chessman_images[BLACK_BISHOP] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_bishop.png");
	chessman_images[BLACK_CANNON] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_cannon.png");
	chessman_images[BLACK_KING] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_king.png");
	chessman_images[BLACK_KING_DIE] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_king_die.png");
	chessman_images[BLACK_KNIGHT] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_knight.png");
	chessman_images[BLACK_PAWN] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_pawn.png");
	chessman_images[BLACK_ROOT] = Gdk::Pixbuf::create_from_file(DATA_DIR"black_root.png");
	chessman_images[RED_ADVISOR] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_advisor.png");
	chessman_images[RED_BISHOP] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_bishop.png");
	chessman_images[RED_CANNON] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_cannon.png");
	chessman_images[RED_KING] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_king.png");
	chessman_images[RED_KING_DIE] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_king_die.png");
	chessman_images[RED_KNIGHT] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_knight.png");
	chessman_images[RED_PAWN] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_pawn.png");
	chessman_images[RED_ROOT] = Gdk::Pixbuf::create_from_file(DATA_DIR"red_root.png");
	chessman_images[SELECTED_CHESSMAN] = Gdk::Pixbuf::create_from_file(DATA_DIR"select.png");
	chessman_images[NULL_CHESSMAN] = Gdk::Pixbuf::create_from_file(DATA_DIR"null.png");
	
	m_engine.add_snapshot(start_fen);
	//m_engine.from_fen(start_fen);
	this->set_events(Gdk::BUTTON_PRESS_MASK);
	this->show_all();
}

Board::~Board()
{
}

void Board::get_grid_size(int& width, int& height)
{
	width = (get_width() - border_width * 2) / 8;
	height = (get_height() - border_width * 2) / 9;
}

Gdk::Point Board::get_coordinate(int pos_x, int pos_y)
{
	int grid_width;
	int grid_height;
	get_grid_size(grid_width, grid_height);
	pos_x = pos_x * grid_width + border_width;
	pos_y = pos_y * grid_height + border_width;

	return Gdk::Point(pos_x, pos_y);
}


Gdk::Point Board::get_position(int pos_x, int pos_y)
{
	int grid_width;
	int grid_height;
	get_grid_size(grid_width, grid_height);
	pos_x -= border_width;
	pos_y -= border_width;
	pos_x += grid_width / 2;
	pos_y += grid_height / 2;
	int x = pos_x / grid_width;
	int y = pos_y / grid_height;
	int offset_x = pos_x - x * grid_width;
	int offset_y = pos_y - y * grid_height;
	if ((offset_x > chessman_width ) || (offset_y > chessman_width ))
		return Gdk::Point(-1, -1);
	return Gdk::Point(x, y);
}

void Board::on_map()
{
	Gtk::DrawingArea::on_map();
	selected_chessman_image = Gdk::Image::create (Gdk::IMAGE_SHARED, get_window()->get_visual(), chessman_width, chessman_width);
}

bool Board::on_expose_event(GdkEventExpose* ev)
{
	draw_bg();
	draw_board();
	//draw_pieces(PieceExample);
	return true;
}

/**处理点击事件*/
bool Board::on_button_press_event(GdkEventButton* ev)
{
	if(ev->type == GDK_BUTTON_PRESS&& ev->button == 1)
	{
		//格式化选中的位置
		draw_select_frame(false);
		Gdk::Point p = get_position(ev->x, ev->y);
		selected_x = p.get_x();
		selected_y = p.get_y();
		selected_chessman = m_engine.get_piece(selected_x, selected_y);
		if (selected_x != -1) {
			draw_select_frame(true);
		}
	}

	return true;
}

void Board::draw_bg()
{
	Gdk::Point p1= get_coordinate(0, 0);
	Gdk::Point p2= get_coordinate(8, 9);

	int width = p2.get_x() - p1.get_x();
	int height = p2.get_y() - p1.get_y();

	int bg_width = bg_image->get_width();
	int bg_height = bg_image->get_height();

	int count_w = get_width() / bg_width + 1;
	int count_h = get_height() / bg_height + 1;
	for (int i = 0; i < count_w; i++) {
		for (int j = 0; j < count_h; j++) {
			bg_image->render_to_drawable(get_window(), get_style()->get_black_gc(),
					0, 0, i * bg_width, j * bg_height, bg_width, bg_height, 
					Gdk::RGB_DITHER_NONE, 0, 0);
		}
	}


	Glib::RefPtr<Gdk::GC> gc = this->get_style()->get_black_gc();
	gc->set_line_attributes(4, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_ROUND);
	get_window()->draw_rectangle(gc, false, 
			p1.get_x() - 8, p1.get_y() - 8,
			width + 8 * 2, height + 8 * 2);

	gc->set_line_attributes(2, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_ROUND);
	this->get_window()->draw_rectangle(gc, false, 
			p1.get_x(), p1.get_y(),
			width, height);

	gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_ROUND);
	int grid_width;
	int grid_height;
	get_grid_size(grid_width, grid_height);

	GdkSegment seg[9];

	for (int i = 0; i < 9; i++) {
		p1 = get_coordinate(0, i);
		p2 = get_coordinate(8, i);
		seg[i].x1 = p1.get_x();
		seg[i].y1 = p1.get_y();
		seg[i].x2 = p2.get_x();
		seg[i].y2 = p2.get_y();
	}
	get_window()->draw_segments(gc, seg, 9);

	for (int i = 0; i < 8; i++) {
		p1 = get_coordinate(i, 0);
		p2 = get_coordinate(i, 4);
		seg[i].x1 = p1.get_x();
		seg[i].y1 = p1.get_y();
		seg[i].x2 = p2.get_x();
		seg[i].y2 = p2.get_y();
	}
	get_window()->draw_segments(gc, seg, 8);

	for (int i = 0; i < 8; i++) {
		p1 = get_coordinate(i, 5);
		p2 = get_coordinate(i, 9);
		seg[i].x1 = p1.get_x();
		seg[i].y1 = p1.get_y();
		seg[i].x2 = p2.get_x();
		seg[i].y2 = p2.get_y();
	}
	get_window()->draw_segments(gc, seg, 8);

	gc->set_line_attributes(2, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_ROUND );

	draw_localize(gc, 0, 3, PLACE_LEFT);
	draw_localize(gc, 8, 3, PLACE_RIGHT);

	for (int i = 0; i < 3; i++) {
		draw_localize(gc, i * 2 + 2, 3, PLACE_ALL);
	}

	draw_localize(gc, 1, 2, PLACE_ALL);
	draw_localize(gc, 7, 2, PLACE_ALL);

	draw_localize(gc, 0, 6, PLACE_LEFT);
	draw_localize(gc, 8, 6, PLACE_RIGHT);

	for (int i = 0; i < 3; i++) {
		draw_localize(gc,  i * 2 + 2, 6, PLACE_ALL);
	}

	draw_localize(gc, 1, 7, PLACE_ALL);
	draw_localize(gc, 7, 7, PLACE_ALL);

	gc->set_line_attributes(1, Gdk::LINE_SOLID, Gdk::CAP_NOT_LAST, Gdk::JOIN_BEVEL );
	draw_palace(gc, 4, 1); 
	draw_palace(gc, 4, 8); 
}

void Board::draw_localize(Glib::RefPtr<Gdk::GC>& gc, int x, int y, int place)
{
	int width;
	int height;
	get_grid_size(width, height);
	width /= 5;
	height /= 5;

	Gdk::Point p = get_coordinate(x, y);

	if (place & PLACE_LEFT) {
		std::vector<Gdk::Point> poss;
		poss.push_back(Gdk::Point(p.get_x() + 5, p.get_y() - height - 4));
		poss.push_back(Gdk::Point(p.get_x() + 5, p.get_y() - 4));
		poss.push_back(Gdk::Point(p.get_x() + 5 + width, p.get_y() - 4));
		this->get_window()->draw_lines(gc, poss);

		poss.clear();
		poss.push_back(Gdk::Point(p.get_x() + 5 + width, p.get_y() + 5));
		poss.push_back(Gdk::Point(p.get_x() + 5, p.get_y() + 5));
		poss.push_back(Gdk::Point(p.get_x() + 5 , p.get_y() + 5 + height));
		this->get_window()->draw_lines(gc, poss);
	}

	if (place & PLACE_RIGHT) {
		std::vector<Gdk::Point> poss;
		poss.push_back(Gdk::Point(p.get_x() - 4 - width, p.get_y() - 4));
		poss.push_back(Gdk::Point(p.get_x() - 4 , p.get_y() - 4));
		poss.push_back(Gdk::Point(p.get_x() - 4 , p.get_y() - 4 - height));
		this->get_window()->draw_lines(gc, poss);
		poss.clear();
		poss.push_back(Gdk::Point(p.get_x() - 4 - width, p.get_y() + 5));
		poss.push_back(Gdk::Point(p.get_x() - 4 , p.get_y() + 5));
		poss.push_back(Gdk::Point(p.get_x() - 4 , p.get_y() + 5 + height));
		this->get_window()->draw_lines(gc, poss);
	}
}

void Board::draw_palace(Glib::RefPtr<Gdk::GC>& gc, int x, int y)
{
	int width;
	int height;
	get_grid_size(width, height);
	Gdk::Point p = get_coordinate(x, y);

	get_window()->draw_line(gc, p.get_x() - width, p.get_y() - height, p.get_x() + width, p.get_y() + height);
	get_window()->draw_line(gc, p.get_x() + width, p.get_y() - height, p.get_x() - width, p.get_y() + height);
}


void Board::draw_chessman(int x, int y, int chessman)
{
	//std::cout << chessman << "  ";
	if(chessman<16)
		return;
	int chess_type = get_chessman_type(chessman);	
	//chess_type--;
	//if (chess_type < 0 )
	//	return;

	Gdk::Point p = get_coordinate(x, y);
	int px = p.get_x() - 57 / 2;
	int py = p.get_y() - 57 / 2;

	chessman_images[chess_type]->render_to_drawable(get_window(), get_style()->get_black_gc(),
			0, 0, px, py, chessman_images[chess_type]->get_width(), chessman_images[chess_type]->get_height(), 
			Gdk::RGB_DITHER_NONE, 0, 0);
}

void Board::draw_select_frame(bool selected)
{
	if (selected_chessman == 0 || selected_x == -1 || selected_y == -1)
		return;

	Gdk::Point p = get_coordinate(selected_x, selected_y);
	int px = p.get_x() - 57 / 2;
	int py = p.get_y() - 57 / 2;


	if (selected) {
		get_window()->copy_to_image(selected_chessman_image, px, py, 0, 0, chessman_width, chessman_width);
		chessman_images[SELECTED_CHESSMAN]->render_to_drawable(get_window(), get_style()->get_black_gc(),
				0, 0, px, py, chessman_images[SELECTED_CHESSMAN]->get_width(), chessman_images[SELECTED_CHESSMAN]->get_height(), 
				Gdk::RGB_DITHER_NONE, 0, 0);
	} else {
		get_window()->draw_image (get_style()->get_black_gc(), selected_chessman_image, 0, 0, px, py, -1, -1);
	}
}

void Board::draw_pieces(const int pieces[])
{
#if 0
	int i,j;
	i = PIECE_START;
	for(;i<PIECE_END;i++)
	{
		int sq = pieces[i];
		if(sq == 0||sq<0x33||sq>0xcb)
			continue;
		draw_chessman(RANK_X(sq),RANK_Y(sq),get_chessman_type(i));
	}
	draw_select_frame(true);
#endif
}

void Board::draw_board()
{
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 10; j++) {
			draw_chessman(i, j, m_engine.get_piece(i, j));
		}
	}
	draw_select_frame(true);
}

void Board::next_move()
{}
void Board::back_move()
{}

