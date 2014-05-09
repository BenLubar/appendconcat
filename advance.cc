#include "util.h"
#include "state.h"

namespace {
	template <typename Elem>
	const Elem & random_element_from_set(const std::unordered_set<Elem> & set) {
		size_t n = random_number(set.size());
		for (auto & elem : set) {
			if (n == 0) {
				return elem;
			}
			--n;
		}
		assert(false);
	}

	const size_t new_site_margin = 4;
	const size_t new_figure_chance = 250;
}

void State::advance(const appendconcat::Time & target_time) {
	while (time_compare(current_time, target_time)) {
		bool change = false;
		appendconcat::Event msg;
		*msg.mutable_id() = random_uuid();

		*msg.mutable_time() = current_time;
		do {
			*msg.mutable_time() = advance_time(msg.time(), 0, 0, 0, random_number(1, 24 * 60 * 60));
			if (time_compare(target_time, msg.time())) {
				*msg.mutable_time() = target_time;
			}

			if (random_number(new_figure_chance) == 0) {
				change = true;
				auto figure = msg.add_figures();

				*figure->mutable_id() = random_uuid();
				*figure->mutable_name() = random_name_figure();
				*figure->mutable_born() = msg.time();
				figure->set_race(appendconcat::Figure::HUMAN);

				switch (random_number(0, 2)) {
				case 0:
					*figure->mutable_location() = random_site_by_type(appendconcat::Site::HOUSE, msg.time());
					break;
				case 1:
					*figure->mutable_location() = random_site_by_type(appendconcat::Site::CAMP, msg.time());
					break;
				case 2:
					*figure->mutable_location() = random_site_by_type(appendconcat::Site::FORTRESS, msg.time());
					break;
				}
			}
		} while (!change && time_compare(msg.time(), target_time));

		add(msg);
	}
}

const appendconcat::UUID & State::random_site_by_type(appendconcat::Site::Type type, const appendconcat::Time & time) {
	auto & sites = sites_by_type[type];

	if (random_number(sites.size() + new_site_margin) < new_site_margin) {
		appendconcat::Event msg;
		*msg.mutable_id() = random_uuid();

		*msg.mutable_time() = time;
		auto site = msg.add_sites();

		switch (type) {
		case appendconcat::Site::REGION:
			if (random_number(0, 9) == 0) {
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::REGION, time);
			}
			break;

		case appendconcat::Site::TOWN:
		case appendconcat::Site::CAMP:
		case appendconcat::Site::LAIR:
			*site->mutable_parent() = random_site_by_type(appendconcat::Site::REGION, time);
			break;

		case appendconcat::Site::FORTRESS:
			if (random_number(0, 1) == 0) {
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::TOWN, time);
			} else {
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::REGION, time);
			}
			break;

		case appendconcat::Site::TOMB:
			switch (random_number(0, 2)) {
			case 0:
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::TOWN, time);
				break;

			case 1:
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::FORTRESS, time);
				break;

			case 2:
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::REGION, time);
				break;
			}
			break;

		case appendconcat::Site::DUNGEON:
			if (random_number(0, 1) == 0) {
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::TOWN, time);
			} else {
				*site->mutable_parent() = random_site_by_type(appendconcat::Site::FORTRESS, time);
			}
			break;

		case appendconcat::Site::HOUSE:
			*site->mutable_parent() = random_site_by_type(appendconcat::Site::TOWN, time);
			break;
		}
		*site->mutable_id() = random_uuid();
		site->set_type(type);
		if (site->type() != appendconcat::Site::HOUSE) {
			*site->mutable_name() = random_name_site(site->type());
		}

		// FIXME: make these numbers more realistic for things other than town->region and town->town
		if (site->has_parent()) {
			auto site_near = site->add_nearby();
			*site_near->mutable_site() = site->parent();
			auto site_distance = site_near->mutable_distance();
			site_distance->set_year(0);
			site_distance->set_month(0);
			site_distance->set_day(random_number(7) + 1);

			auto parent_site = msg.add_sites();
			*parent_site->mutable_id() = site->parent();
			auto parent_near = parent_site->add_nearby();
			*parent_near->mutable_site() = site->id();
			auto parent_distance = parent_near->mutable_distance();
			parent_distance->set_year(0);
			parent_distance->set_month(random_number(4) + 1);
			parent_distance->set_day(random_number(28));
		}

		for (auto other : sites_by_parent[site->parent()]) {
			auto site_site_from = msg.add_sites();
			*site_site_from->mutable_id() = other;
			auto site_near_from = site_site_from->add_nearby();
			*site_near_from->mutable_site() = site->id();
			auto site_distance_from = site_near_from->mutable_distance();
			site_distance_from->set_year(0);
			site_distance_from->set_month(random_number(3));
			site_distance_from->set_day(random_number(28));
			site_distance_from->set_second(random_number(24 * 60 * 60));

			auto site_near_to = site->add_nearby();
			*site_near_to->mutable_site() = other;
			auto site_distance_to = site_near_to->mutable_distance();
			site_distance_to->set_year(0);
			site_distance_to->set_month(random_number(3));
			site_distance_to->set_day(random_number(28));
			site_distance_to->set_second(random_number(24 * 60 * 60));
		}

		add(msg);

		return site->id();
	}
	return random_element_from_set<appendconcat::UUID>(sites);
}
