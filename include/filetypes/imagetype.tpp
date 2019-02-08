
template<class To, class From>
To* convert_image(From* img) {
    if (From::image_type == "SingleImageType") {
        return from_single_image<To, From>(img);
    } else if (From::image_type == "MultiImageType") {
        return from_multi_image<To, From>(img);
    } else {
        throw std::runtime_error("Unrecognized image type to convert from");
    }
}

template<class To, class From>
To* from_single_image(From* img) {
    if (To::image_type == "SingleImageType") {
        return new To(img->get_image());
    } else if (To::image_type == "MultiImageType") {
        Image* new_img = new Image(img->get_image());
        return new To(new_img, 1);
    }
}

template<class To, class From>
To* from_multi_image(From* img) {
    if (To::image_type == "SingleImageType") {
        if (img->num_images() > 1) {
            throw std::runtime_error("Cannot convert multiple images into a single image");
        }
        return new To(img->get_images()[0]);
    } else if (To::image_type == "MultiImageType"){
        return new To(img->get_images(), img->num_images());
    }
}
