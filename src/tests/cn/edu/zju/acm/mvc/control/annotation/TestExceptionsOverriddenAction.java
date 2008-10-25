
package cn.edu.zju.acm.mvc.control.annotation;

import cn.edu.zju.acm.mvc.control.annotation.Exceptions;
import cn.edu.zju.acm.mvc.control.annotation.OneException;

@Exceptions( {@OneException(exception = Exception.class, result = "jsp"),
              @OneException(exception = IllegalArgumentException.class, result = "jsp"),})
public class TestExceptionsOverriddenAction extends TestExceptionActionBase {
}
