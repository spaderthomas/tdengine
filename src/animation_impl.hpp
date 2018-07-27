void Animation::next_frame() {
	icur_frame = (icur_frame + 1) % frames.size();
}