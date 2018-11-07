void Animation::next_frame() {
	icur_frame = (icur_frame + 1) % frames.size();
	time_to_next_frame = 8.f / 60.f;
}